#include "graphics/vulkan/vulkanRenderer.hpp"

#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <vector>

#include "graphics/renderAPI.hpp"
#include "graphics/vulkan/SwapchainSupportDetails.hpp"

namespace flex {
VulkanRenderer::VulkanRenderer(RenderWindow const &window) {
  if (window.getRenderAPI() != RenderAPI::Vulkan) {
    throw InvalidRenderAPIException{
        "Can't create vulkan renderer if window is not initialized with the "
        "Vulkan render API"};
  }

  createVulkanInstance(window);
  mSurface = window.getDrawableVulkanSurface(mInstance);
  selectPhysicalDevice();
  mQueueFamilyIndices = QueueFamilyIndices{mPhysicalDevice, mSurface};
  createVulkanDevice();
  retrieveQueues();
  mSwapchain.createSwapchain(mPhysicalDevice, mDevice, window, mSurface, mQueueFamilyIndices);
  createRenderPass();
  mPipeline.createPipeline(mDevice, mSwapchain, mRenderPass);
  mSwapchain.createFrameBuffers(mDevice, mRenderPass);
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
  mCurrentFrame = 0;
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
  vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
  vkDestroyDevice(mDevice, nullptr);
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  vkDestroyInstance(mInstance, nullptr);
}

void VulkanRenderer::createVulkanInstance(RenderWindow const &window) {
  std::string const title = window.getTitle();

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = title.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Flex Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  std::vector<char const *> enabledExtensions = window.getRequiredVulkanExtensions();
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

  vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);
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
                                                VkSurfaceKHR const &vulkanSurface) const {

  unsigned int score = 1u;

  VkPhysicalDeviceProperties physicalDeviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

  if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000u;
  }

  if (QueueFamilyIndices const queueFamilyIndices{physicalDevice, vulkanSurface};
      !queueFamilyIndices.isComplete()) {
    return 0u;
  }

  if (!physicalDeviceSupportsRequiredExtensions(physicalDevice)) {
    return 0u;
  }

  if (SwapchainSupportDetails const swapchainSupportDetails{physicalDevice, vulkanSurface};
      !swapchainSupportDetails.isUsable()) {
    return 0u;
  }

  return score;
}

bool VulkanRenderer::physicalDeviceSupportsRequiredExtensions(
    VkPhysicalDevice const &physicalDevice) const {

  uint32_t availableExtensionsCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions{availableExtensionsCount};
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount,
                                       availableExtensions.data());

  for (std::string const &requiredExtensionName : mRequiredDeviceExtensionsNames) {
    bool extensionFound = false;
    for (VkExtensionProperties const &availableExtension : availableExtensions) {
      if (std::strcmp(requiredExtensionName.c_str(), availableExtension.extensionName) == 0) {
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
  std::set<uint32_t> uniqueQueueFamilyIndices = {mQueueFamilyIndices.graphics.value(),
                                                 mQueueFamilyIndices.transfer.value(),
                                                 mQueueFamilyIndices.present.value()};

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

  vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice);
}

void VulkanRenderer::retrieveQueues() {
  vkGetDeviceQueue(mDevice, mQueueFamilyIndices.graphics.value(), 0, &mGraphicsQueue);
  vkGetDeviceQueue(mDevice, mQueueFamilyIndices.present.value(), 0, &mPresentQueue);
  vkGetDeviceQueue(mDevice, mQueueFamilyIndices.transfer.value(), 0, &mTransferQueue);
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

  VkSubpassDescription subpassDescription{};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &colorAttachmentReference;

  VkSubpassDependency subpassDependency{};
  subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependency.dstSubpass = 0;
  subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.srcAccessMask = 0;
  subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = 1;
  renderPassCreateInfo.pAttachments = &colorAttachment;
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpassDescription;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &subpassDependency;

  vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mRenderPass);
}

void VulkanRenderer::createCommandPool() {
  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.queueFamilyIndex = mQueueFamilyIndices.graphics.value();

  vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &mCommandPool);
}

void VulkanRenderer::createCommandBuffers() {

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = mCommandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount =
      static_cast<uint32_t>(mSwapchain.framebuffers.size());

  mCommandBuffers.resize(mSwapchain.framebuffers.size());
  vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, mCommandBuffers.data());

  VkRect2D renderArea;
  renderArea.offset = {0, 0};
  renderArea.extent = mSwapchain.extent;

  VkClearValue clearValue;
  clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

  for (size_t i = 0; i < mCommandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo);

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = mRenderPass;
    renderPassBeginInfo.framebuffer = mSwapchain.framebuffers[i];
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline.pipeline);
    vkCmdDraw(mCommandBuffers[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(mCommandBuffers[i]);
    vkEndCommandBuffer(mCommandBuffers[i]);
  }
}

void VulkanRenderer::createSyncObjects() {
  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (unsigned int i = 0; i < mMaxFramesInFlight; i++) {
    vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &mImageAvailableSemaphores[i]);
    vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &mRenderFinishedSemaphores[i]);
    vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &mInFlightFences[i]);
  }

  mImagesInFlight.resize(mSwapchain.images.size(), nullptr);
}

void VulkanRenderer::draw() {

  vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(mDevice, mSwapchain.vulkanSwapchain, UINT64_MAX,
                        mImageAvailableSemaphores[mCurrentFrame], nullptr, &imageIndex);

  if (mImagesInFlight[imageIndex] != nullptr) {
    vkWaitForFences(mDevice, 1, &mImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
  }

  mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame];

  VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pWaitDstStageMask = &waitStage;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];

  vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);
  vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]);

  std::array<VkSwapchainKHR, 1> presentSwapchain{mSwapchain.vulkanSwapchain};

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];
  presentInfo.swapchainCount = static_cast<uint32_t>(presentSwapchain.size());
  presentInfo.pSwapchains = presentSwapchain.data();
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(mPresentQueue, &presentInfo);

  mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
}

} // namespace flex
