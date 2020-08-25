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
  mDevice.waitIdle();

  for (unsigned int i = 0; i < mMaxFramesInFlight; i++) {
    mDevice.destroySemaphore(mRenderFinishedSemaphores[i]);
    mDevice.destroySemaphore(mImageAvailableSemaphores[i]);
    mDevice.destroyFence(mInFlightFences[i]);
  }
  mDevice.destroyCommandPool(mCommandPool);
  mPipeline.destroy(mDevice);
  mSwapChain.destroy(mDevice);
  mDevice.destroyRenderPass(mRenderPass);
  mDevice.destroy();
  mInstance.destroySurfaceKHR(mSurface);
  mInstance.destroy();
}

void VulkanRenderer::createVulkanInstance(Window const &window) {
  vk::ApplicationInfo appInfo{window.getTitle().c_str(), VK_MAKE_VERSION(1, 0, 0), "Flex Engine",
                              VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_0};

  std::vector<char const *> enabledExtensions = window.getRequiredVulkanExtensions();

  std::vector<char const *> enabledLayers{};

#ifndef NDEBUG
  enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

  vk::InstanceCreateInfo const instanceCreateInfo({}, &appInfo, enabledLayers, enabledExtensions);

  mInstance = createInstance(instanceCreateInfo);
}

void VulkanRenderer::selectPhysicalDevice() {
  std::vector<vk::PhysicalDevice> availablePhysicalDevices = mInstance.enumeratePhysicalDevices();

  std::multimap<int, vk::PhysicalDevice> rankedPhysicalDevices = {};

  for (vk::PhysicalDevice const &physicalDevice : availablePhysicalDevices) {
    unsigned int deviceScore = ratePhysicalDevice(physicalDevice, mSurface);
    rankedPhysicalDevices.insert(std::make_pair(deviceScore, physicalDevice));
  }

  if (rankedPhysicalDevices.rbegin()->first > 0) {
    mPhysicalDevice = rankedPhysicalDevices.rbegin()->second;
  } else {
    throw std::runtime_error("No suitable device supporting vulkan found");
  }
}

unsigned int VulkanRenderer::ratePhysicalDevice(vk::PhysicalDevice const &physicalDevice,
                                                vk::SurfaceKHR const &vulkanSurface) const {

  unsigned int score = 1u;

  vk::PhysicalDeviceProperties const physicalDeviceProperties = physicalDevice.getProperties();

  if (physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
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
    vk::PhysicalDevice const &physicalDevice) const {
  std::vector<vk::ExtensionProperties> availableExtensions =
      physicalDevice.enumerateDeviceExtensionProperties();

  for (std::string const &requiredExtensionName : mRequiredDeviceExtensionsNames) {
    bool extensionFound = false;
    for (vk::ExtensionProperties const &availableExtension : availableExtensions) {
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

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
  queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());

  for (uint32_t const &queueFamilyIndex : uniqueQueueFamilyIndices) {
    queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{{}, queueFamilyIndex, 1, &queuePriority});
  }

  vk::PhysicalDeviceFeatures enabledDeviceFeatures{};

  vk::DeviceCreateInfo const deviceCreateInfo{
      {}, queueCreateInfos, {}, mRequiredDeviceExtensionsNames, &enabledDeviceFeatures};
  mDevice = mPhysicalDevice.createDevice(deviceCreateInfo);
}

void VulkanRenderer::retrieveQueues() {
  mGraphicsQueue = mDevice.getQueue(mQueueFamilyIndices.graphics.value(), 0);
  mPresentQueue = mDevice.getQueue(mQueueFamilyIndices.present.value(), 0);
  mTransferQueue = mDevice.getQueue(mQueueFamilyIndices.transfer.value(), 0);
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
  mRenderPass = mDevice.createRenderPass(renderPassCreateInfo);
}

void VulkanRenderer::createCommandPool() {
  vk::CommandPoolCreateInfo const commandPoolCreateInfo{{}, mQueueFamilyIndices.graphics.value()};
  mCommandPool = mDevice.createCommandPool(commandPoolCreateInfo);
}

void VulkanRenderer::createCommandBuffers() {

  vk::CommandBufferAllocateInfo const commandBufferAllocateInfo{
      mCommandPool, vk::CommandBufferLevel::ePrimary,
      static_cast<uint32_t>(mSwapChain.framebuffers.size())};

  mCommandBuffers = mDevice.allocateCommandBuffers(commandBufferAllocateInfo);

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
  vk::SemaphoreCreateInfo const semaphoreCreateInfo{};
  vk::FenceCreateInfo const fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};

  for (unsigned int i = 0; i < mMaxFramesInFlight; i++) {
    mImageAvailableSemaphores[i] = mDevice.createSemaphore(semaphoreCreateInfo);
    mRenderFinishedSemaphores[i] = mDevice.createSemaphore(semaphoreCreateInfo);
    mInFlightFences[i] = mDevice.createFence(fenceCreateInfo);
  }

  mImagesInFlight.resize(mSwapChain.images.size(), vk::Fence{});
}

void VulkanRenderer::draw() {

  mDevice.waitForFences(mInFlightFences[mCurrentFrame], true, UINT64_MAX);

  uint32_t imageIndex;
  mDevice.acquireNextImageKHR(mSwapChain.swapChain, UINT64_MAX,
                              mImageAvailableSemaphores[mCurrentFrame], nullptr, &imageIndex);

  if (mImagesInFlight[imageIndex] != vk::Fence{}) {
    static_cast<void>(mDevice.waitForFences(mImagesInFlight, true, UINT64_MAX));
  }

  mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame];

  vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

  std::array<vk::Semaphore, 1> waitSemaphore{mImageAvailableSemaphores[mCurrentFrame]};
  std::array<vk::Semaphore, 1> signalSemaphore{mRenderFinishedSemaphores[mCurrentFrame]};
  std::array<vk::CommandBuffer, 1> commandBuffers{mCommandBuffers[imageIndex]};

  vk::SubmitInfo submitInfo{waitSemaphore, waitStage, commandBuffers, signalSemaphore};

  mDevice.resetFences(mInFlightFences[mCurrentFrame]);
  mGraphicsQueue.submit(submitInfo, mInFlightFences[mCurrentFrame]);

  waitSemaphore = std::array<vk::Semaphore, 1>{mRenderFinishedSemaphores[mCurrentFrame]};
  std::array<vk::SwapchainKHR, 1> presentSwapChain{mSwapChain.swapChain};

  vk::PresentInfoKHR presentInfo{waitSemaphore, presentSwapChain, imageIndex};

  static_cast<void>(mPresentQueue.presentKHR(&presentInfo));

  mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
}

} // namespace flex
