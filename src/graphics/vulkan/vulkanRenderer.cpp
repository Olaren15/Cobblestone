#include "graphics/vulkan/vulkanRenderer.hpp"

#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <vector>

#include "graphics/renderAPI.hpp"
#include "graphics/vulkan/SwapChainSupportDetails.hpp"

namespace flex {
VulkanRenderer::VulkanRenderer(Window const &window) {
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
  mSwapChain = SwapChain{mDevice, window, mSurface,
                         SwapChainSupportDetails{mPhysicalDevice, mSurface}, mQueueFamilyIndices};
  createRenderPass();
  mPipeline = Pipeline{mDevice, mSwapChain, mRenderPass};
  mSwapChain.createFrameBuffers(mDevice, mRenderPass);
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
  mSwapChain.destroy(mDevice);
  vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
  vkDestroyDevice(mDevice, nullptr);
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  vkDestroyInstance(mInstance, nullptr);
}

void VulkanRenderer::createVulkanInstance(Window const &window) {
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

  if (SwapChainSupportDetails const swapChainSupportDetails{physicalDevice, vulkanSurface};
      !swapChainSupportDetails.isUsable()) {
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
  vk::Device device{mDevice};

  mGraphicsQueue = device.getQueue(mQueueFamilyIndices.graphics.value(), 0);
  mPresentQueue = device.getQueue(mQueueFamilyIndices.present.value(), 0);
  mTransferQueue = device.getQueue(mQueueFamilyIndices.transfer.value(), 0);
}

void VulkanRenderer::createRenderPass() {
  vk::AttachmentDescription colorAttachment{{},
                                            mSwapChain.format,
                                            vk::SampleCountFlagBits::e1,
                                            vk::AttachmentLoadOp::eClear,
                                            vk::AttachmentStoreOp::eStore,
                                            vk::AttachmentLoadOp::eDontCare,
                                            vk::AttachmentStoreOp::eDontCare,
                                            vk::ImageLayout::eUndefined,
                                            vk::ImageLayout::ePresentSrcKHR};

  vk::AttachmentReference colorAttachmentReference{0, vk::ImageLayout::eColorAttachmentOptimal};

  vk::SubpassDescription subpassDescription{
      {}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentReference, {}, {}, {}};

  vk::SubpassDependency subpassDependency{VK_SUBPASS_EXTERNAL,
                                          0,
                                          vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                          vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                          vk::AccessFlagBits{0},
                                          vk::AccessFlagBits::eColorAttachmentWrite};

  vk::RenderPassCreateInfo const renderPassCreateInfo{
      {}, colorAttachment, subpassDescription, subpassDependency};

  vk::Device device{mDevice};
  mRenderPass = device.createRenderPass(renderPassCreateInfo);
}

void VulkanRenderer::createCommandPool() {
  vk::CommandPoolCreateInfo const commandPoolCreateInfo{{}, mQueueFamilyIndices.graphics.value()};

  vk::Device device{mDevice};
  mCommandPool = device.createCommandPool(commandPoolCreateInfo);
}

void VulkanRenderer::createCommandBuffers() {
  vk::Device device{mDevice};

  vk::CommandBufferAllocateInfo const commandBufferAllocateInfo{
      mCommandPool, vk::CommandBufferLevel::ePrimary,
      static_cast<uint32_t>(mSwapChain.framebuffers.size())};

  mCommandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);

  vk::Rect2D const renderArea{vk::Offset2D{0, 0}, mSwapChain.extent};
  vk::ClearValue clearValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};

  for (size_t i = 0; i < mCommandBuffers.size(); i++) {
    vk::CommandBufferBeginInfo beginInfo{};
    mCommandBuffers[i].begin(beginInfo);

    vk::RenderPassBeginInfo renderPassBeginInfo{mRenderPass, mSwapChain.framebuffers[i], renderArea,
                                                clearValue};
    mCommandBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    mCommandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline.pipeline);
    mCommandBuffers[i].draw(3, 1, 0, 0);

    mCommandBuffers[i].endRenderPass();
    mCommandBuffers[i].end();
  }
}

void VulkanRenderer::createSyncObjects() {
  vk::Device device{mDevice};

  vk::SemaphoreCreateInfo const semaphoreCreateInfo{};
  vk::FenceCreateInfo const fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};

  for (unsigned int i = 0; i < mMaxFramesInFlight; i++) {
    mImageAvailableSemaphores[i] = device.createSemaphore(semaphoreCreateInfo);
    mRenderFinishedSemaphores[i] = device.createSemaphore(semaphoreCreateInfo);
    mInFlightFences[i] = device.createFence(fenceCreateInfo);
  }

  mImagesInFlight.resize(mSwapChain.images.size(), vk::Fence{});
}

void VulkanRenderer::draw() {
  vk::Device device{mDevice};

  device.waitForFences(mInFlightFences[mCurrentFrame], true, UINT64_MAX);

  uint32_t imageIndex;
  device.acquireNextImageKHR(mSwapChain.swapChain, UINT64_MAX,
                             mImageAvailableSemaphores[mCurrentFrame], nullptr, &imageIndex);

  if (mImagesInFlight[imageIndex] != vk::Fence{}) {
    static_cast<void>(device.waitForFences(mImagesInFlight, true, UINT64_MAX));
  }

  mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame];

  vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

  std::array<vk::Semaphore, 1> waitSemaphore{mImageAvailableSemaphores[mCurrentFrame]};
  std::array<vk::Semaphore, 1> signalSemaphore{mRenderFinishedSemaphores[mCurrentFrame]};
  std::array<vk::CommandBuffer, 1> commandBuffers{mCommandBuffers[imageIndex]};

  vk::SubmitInfo submitInfo{waitSemaphore, waitStage, commandBuffers, signalSemaphore};

  device.resetFences(mInFlightFences[mCurrentFrame]);
  mGraphicsQueue.submit(submitInfo, mInFlightFences[mCurrentFrame]);

  waitSemaphore = std::array<vk::Semaphore, 1>{mRenderFinishedSemaphores[mCurrentFrame]};
  std::array<vk::SwapchainKHR, 1> presentSwapChain{mSwapChain.swapChain};

  vk::PresentInfoKHR presentInfo{waitSemaphore, presentSwapChain, imageIndex};

  static_cast<void>(mPresentQueue.presentKHR(&presentInfo));

  mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
}

} // namespace flex
