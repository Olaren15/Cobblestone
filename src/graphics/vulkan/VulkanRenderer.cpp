#include "graphics/vulkan/VulkanRenderer.hpp"

#include <cstdint>
#include <map>
#include <set>
#include <vector>

#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {
VulkanRenderer::VulkanRenderer(RenderWindow const &window, Camera const &camera)
    : mWindow(window), mCamera(camera), mDepthBufferImage(mMemoryManager) {
  if (window.getRenderAPI() != RenderAPI::Vulkan) {
    throw InvalidRenderAPIException{
        "Can't create vulkan renderer if window is not initialized with the "
        "Vulkan render API"};
  }

  createVulkanInstance();
  mSurface = window.getDrawableVulkanSurface(mInstance);
  selectPhysicalDevice();
  mQueues.buildQueueFamilyIndices(mPhysicalDevice, mSurface);
  createVulkanDevice();
  mQueues.retrieveQueues(mDevice);
  mMemoryManager.initialize(mInstance, mPhysicalDevice, mDevice, mQueues);
  mSwapchain.createSwapchain(mPhysicalDevice, mDevice, window, mSurface, mQueues.familyIndices);
  createRenderPass();
  mDepthBufferImage = mMemoryManager.createDepthBufferImage(mSwapchain.extent);
  mPipeline.createPipeline(mDevice, mRenderPass);
  mSwapchain.createFrameBuffers(mDevice, mRenderPass, mDepthBufferImage);
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
}

VulkanRenderer::~VulkanRenderer() {
  vkDeviceWaitIdle(mDevice);

  for (unsigned int i = 0; i < mMaxFramesInFlight; i++) {
    vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
    vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
  }
  vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
  mPipeline.destroy(mDevice);
  mSwapchain.destroy(mDevice);
  mMemoryManager.destroyImage(mDepthBufferImage);
  vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
  mMemoryManager.destroy();
  vkDestroyDevice(mDevice, nullptr);
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  vkDestroyInstance(mInstance, nullptr);
}

void VulkanRenderer::createVulkanInstance() {
  std::string const title = mWindow.getTitle();

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = title.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Flex Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  std::vector<char const *> enabledExtensions = mWindow.getRequiredVulkanExtensions();
  std::vector<char const *> enabledLayers{};

#ifndef NDEBUG
  enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

  VkInstanceCreateInfo instanceCreateInfo{};
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pApplicationInfo = &appInfo;
  instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
  instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
  instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
  instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

  validateVkResult(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance));
}

void VulkanRenderer::selectPhysicalDevice() {
  uint32_t physicalDeviceCount;
  vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);
  std::vector<VkPhysicalDevice> availablePhysicalDevices{physicalDeviceCount};
  vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, availablePhysicalDevices.data());

  std::multimap<int, VkPhysicalDevice> rankedPhysicalDevices = {};

  for (VkPhysicalDevice const &physicalDevice : availablePhysicalDevices) {
    unsigned int deviceScore = ratePhysicalDevice(physicalDevice, mSurface);
    rankedPhysicalDevices.insert(std::make_pair(deviceScore, physicalDevice));
  }

  if (rankedPhysicalDevices.rbegin()->first > 0) {
    mPhysicalDevice = rankedPhysicalDevices.rbegin()->second;
  } else {
    throw std::runtime_error("No suitable device supporting vulkan found");
  }
}

unsigned int VulkanRenderer::ratePhysicalDevice(VkPhysicalDevice const &physicalDevice,
                                                VkSurfaceKHR const &vulkanSurface) {

  unsigned int score = 1u;

  VkPhysicalDeviceProperties physicalDeviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

  if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000u;
  }

  if (VulkanQueueFamilyIndices const queueFamilyIndices{physicalDevice, vulkanSurface};
      !queueFamilyIndices.isComplete()) {
    return 0u;
  }

  if (!physicalDeviceSupportsRequiredExtensions(physicalDevice)) {
    return 0u;
  }

  if (VulkanSwapchainSupportDetails const swapchainSupportDetails{physicalDevice, vulkanSurface};
      !swapchainSupportDetails.isUsable()) {
    return 0u;
  }

  return score;
}

bool VulkanRenderer::physicalDeviceSupportsRequiredExtensions(
    VkPhysicalDevice const &physicalDevice) {

  uint32_t availableExtensionsCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions{availableExtensionsCount};
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount,
                                       availableExtensions.data());

  for (std::string const &requiredExtensionName : mRequiredDeviceExtensionsNames) {
    bool extensionFound = false;
    for (VkExtensionProperties const &availableExtension : availableExtensions) {
      if (strcmp(requiredExtensionName.c_str(), availableExtension.extensionName) == 0) {
        extensionFound = true;
      }
    }
    if (!extensionFound) {
      return false;
    }
  }

  return true;
}

void VulkanRenderer::createVulkanDevice() {
  float queuePriority = 1.0f;
  std::set<uint32_t> uniqueQueueFamilyIndices = mQueues.familyIndices.getUniqueIndices();

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
  queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());

  for (uint32_t const &queueFamilyIndex : uniqueQueueFamilyIndices) {
    VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(deviceQueueCreateInfo);
  }

  VkPhysicalDeviceFeatures enabledDeviceFeatures{};

  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.enabledExtensionCount =
      static_cast<uint32_t>(mRequiredDeviceExtensionsNames.size());
  deviceCreateInfo.ppEnabledExtensionNames = mRequiredDeviceExtensionsNames.data();
  deviceCreateInfo.pEnabledFeatures = &enabledDeviceFeatures;

  validateVkResult(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice));
}

void VulkanRenderer::createRenderPass() {

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = static_cast<VkFormat>(mSwapchain.format);
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentReference;
  colorAttachmentReference.attachment = 0;
  colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = VulkanImage::getDepthBufferFormat(mPhysicalDevice);
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentReference{};
  depthAttachmentReference.attachment = 1;
  depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDescription{};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &colorAttachmentReference;
  subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

  VkSubpassDependency subpassDependency{};
  subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependency.dstSubpass = 0;
  subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.srcAccessMask = 0;
  subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassCreateInfo.pAttachments = attachments.data();
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpassDescription;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &subpassDependency;

  validateVkResult(vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mRenderPass));
}

void VulkanRenderer::createCommandPool() {
  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  commandPoolCreateInfo.queueFamilyIndex = mQueues.familyIndices.graphics.value();

  validateVkResult(vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &mCommandPool));
}

void VulkanRenderer::createCommandBuffers() {
  mCommandBuffers.resize(mSwapchain.framebuffers.size());

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = mCommandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

  validateVkResult(
      vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, mCommandBuffers.data()));
}

void VulkanRenderer::createSyncObjects() {
  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (unsigned int i = 0; i < mMaxFramesInFlight; i++) {
    validateVkResult(
        vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &mImageAvailableSemaphores[i]));
    validateVkResult(
        vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &mRenderFinishedSemaphores[i]));
    validateVkResult(vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &mInFlightFences[i]));
  }

  mImagesInFlight.resize(mSwapchain.images.size(), mInFlightFences[0]);
}

void VulkanRenderer::handleFrameBufferResize() {
  if (mWindow.hasFocus()) {
    mState.doNotRender = false;

    validateVkResult(vkDeviceWaitIdle(mDevice));
    vkFreeCommandBuffers(mDevice, mCommandPool, static_cast<uint32_t>(mCommandBuffers.size()),
                         mCommandBuffers.data());

    mSwapchain.handleFrameBufferResize(mPhysicalDevice, mDevice, mWindow, mSurface,
                                       mQueues.familyIndices, mRenderPass, mDepthBufferImage);

    createCommandBuffers();
  } else {
    mState.doNotRender = true;
  }
}

bool VulkanRenderer::acquireNextFrame() {
  if (mState.doNotRender) {
    // check if we can render again
    handleFrameBufferResize();
    if (mState.doNotRender) {
      return false;
    }
  }

  if (VkResult fenceStatus = vkGetFenceStatus(mDevice, mInFlightFences[mState.currentFrame]);
      fenceStatus == VK_NOT_READY) {
    // fence is not ready
    return false;
  } else {
    validateVkResult(fenceStatus);
  }

  // do no try to acquire another image if we already have a valid one
  if (!mState.acquiredImageStillValid) {
    if (VkResult result = vkAcquireNextImageKHR(mDevice, mSwapchain.swapchain, 0,
                                                mImageAvailableSemaphores[mState.currentFrame],
                                                VK_NULL_HANDLE, &mState.imageIndex);
        result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      handleFrameBufferResize();
      // return immediately because the acquired frame was not valid
      return false;
    } else if (result == VK_NOT_READY || result == VK_TIMEOUT) {
      // Since we do not wait, it is possible that the next image is not ready
      return false;
    } else {
      // Image acquired
      validateVkResult(result);
      mState.acquiredImageStillValid = true;
    }
  }

  if (VkResult fenceStatus = vkGetFenceStatus(mDevice, mImagesInFlight[mState.imageIndex]);
      fenceStatus == VK_NOT_READY) {
    return false;
  } else {
    validateVkResult(fenceStatus);
  }

  mImagesInFlight[mState.imageIndex] = mInFlightFences[mState.currentFrame];

  return true;
}

void VulkanRenderer::startDraw() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  validateVkResult(vkBeginCommandBuffer(mCommandBuffers[mState.imageIndex], &beginInfo));

  std::array<VkViewport, 1> viewport{};
  viewport[0].x = 0.0f;
  viewport[0].y = 0.0f;
  viewport[0].width = static_cast<float>(mSwapchain.extent.width);
  viewport[0].height = static_cast<float>(mSwapchain.extent.height);
  viewport[0].minDepth = 0.0f;
  viewport[0].maxDepth = 1.0f;

  std::array<VkRect2D, 1> scissors{};
  scissors[0].offset = {0, 0};
  scissors[0].extent = mSwapchain.extent;

  // dynamic states
  vkCmdSetViewport(mCommandBuffers[mState.imageIndex], 0, static_cast<uint32_t>(viewport.size()),
                   viewport.data());
  vkCmdSetScissor(mCommandBuffers[mState.imageIndex], 0, static_cast<uint32_t>(scissors.size()),
                  scissors.data());

  VkRect2D renderArea;
  renderArea.offset = {0, 0};
  renderArea.extent = mSwapchain.extent;

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = mRenderPass;
  renderPassBeginInfo.framebuffer = mSwapchain.framebuffers[mState.imageIndex];
  renderPassBeginInfo.renderArea = renderArea;
  renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassBeginInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(mCommandBuffers[mState.imageIndex], &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  glm::mat4 cameraView =
      mCamera.getViewMatrix(static_cast<float>(mSwapchain.extent.width) / mSwapchain.extent.height);
  vkCmdPushConstants(mCommandBuffers[mState.imageIndex], mPipeline.pipelineLayout,
                     VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof cameraView, &cameraView);

  vkCmdBindPipeline(mCommandBuffers[mState.imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
                    mPipeline.pipeline);
}

void VulkanRenderer::drawMesh(Mesh &mesh) {
  if (!mesh.getVulkanBuffer().has_value()) {
    mesh.setVulkanBuffer(mMemoryManager.buildMeshBuffer(mesh));
  }

  vkCmdBindIndexBuffer(mCommandBuffers[mState.imageIndex], mesh.getVulkanBuffer()->buffer, 0,
                       VK_INDEX_TYPE_UINT32);

  std::array<VkDeviceSize, 1> vertexBufferOffset{mesh.getIndicesSize()};
  vkCmdBindVertexBuffers(mCommandBuffers[mState.imageIndex], 0, 1, &mesh.getVulkanBuffer()->buffer,
                         vertexBufferOffset.data());

  vkCmdDrawIndexed(mCommandBuffers[mState.imageIndex],
                   static_cast<uint32_t>(mesh.getIndices().size()), 1, 0, 0, 0);
}

void VulkanRenderer::endDraw() {
  vkCmdEndRenderPass(mCommandBuffers[mState.imageIndex]);
  validateVkResult(vkEndCommandBuffer(mCommandBuffers[mState.imageIndex]));
}

void VulkanRenderer::present() {

  VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pWaitDstStageMask = &waitStage;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mState.currentFrame];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mState.currentFrame];
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBuffers[mState.imageIndex];

  validateVkResult(vkResetFences(mDevice, 1, &mInFlightFences[mState.currentFrame]));
  validateVkResult(
      vkQueueSubmit(mQueues.graphics, 1, &submitInfo, mInFlightFences[mState.currentFrame]));

  std::array<VkSwapchainKHR, 1> presentSwapchain{mSwapchain.swapchain};

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[mState.currentFrame];
  presentInfo.swapchainCount = static_cast<uint32_t>(presentSwapchain.size());
  presentInfo.pSwapchains = presentSwapchain.data();
  presentInfo.pImageIndices = &mState.imageIndex;

  validateVkResult(vkQueuePresentKHR(mQueues.present, &presentInfo));

  mState.currentFrame = (mState.currentFrame + 1) % mMaxFramesInFlight;

  // allows us to start preparing for the next frame
  mState.acquiredImageStillValid = false;
}

void VulkanRenderer::stop() { vkDeviceWaitIdle(mDevice); }

} // namespace flex
