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

  mVulkanInstance = createVulkanInstance(window);
  mDrawingSurface = window.getDrawableVulkanSurface(mVulkanInstance);
  mPhysicalDevice = selectPhysicalDevice(mVulkanInstance, mDrawingSurface);
  mQueueFamilyIndices = QueueFamilyIndices{mPhysicalDevice, mDrawingSurface};
  mDevice = createVulkanDevice(mPhysicalDevice, mQueueFamilyIndices);
  mGraphicsQueue = retrieveQueue(mDevice, mQueueFamilyIndices, QueueFamily::Graphics);
  mPresentQueue = retrieveQueue(mDevice, mQueueFamilyIndices, QueueFamily::Present);
  mSwapChain =
      SwapChain{mDevice, window, mDrawingSurface,
                SwapChainSupportDetails{mPhysicalDevice, mDrawingSurface}, mQueueFamilyIndices};
  mPipeline = Pipeline{mDevice, mSwapChain};
  mSwapChain.createFrameBuffers(mDevice, mPipeline.renderPass);
}

VulkanRenderer::~VulkanRenderer() {
  mPipeline.destroy(mDevice);
  mSwapChain.destroy(mDevice);
  mDevice.destroy();
  mVulkanInstance.destroySurfaceKHR(mDrawingSurface);
  mVulkanInstance.destroy();
}

vk::Instance VulkanRenderer::createVulkanInstance(Window const &window) const {
  vk::ApplicationInfo appInfo{window.getTitle().c_str(), VK_MAKE_VERSION(1, 0, 0), "Flex Engine",
                              VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_0};

  std::vector<char const *> enabledExtensions = window.getRequiredVulkanExtensions();

  std::vector<char const *> enabledLayers{};

  // ReSharper disable once CppUnreachableCode
  if (mEnableValidationLayers) {
    enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
  }

  vk::InstanceCreateInfo const instanceCreateInfo({}, &appInfo, enabledLayers, enabledExtensions);

  return createInstance(instanceCreateInfo);
}

vk::PhysicalDevice VulkanRenderer::selectPhysicalDevice(vk::Instance const &vulkanInstance,
                                                        vk::SurfaceKHR const &vulkanSurface) const {
  std::vector<vk::PhysicalDevice> availablePhysicalDevices =
      vulkanInstance.enumeratePhysicalDevices();

  std::multimap<int, vk::PhysicalDevice> rankedPhysicalDevices = {};

  for (vk::PhysicalDevice const &physicalDevice : availablePhysicalDevices) {
    unsigned int deviceScore = ratePhysicalDevice(physicalDevice, vulkanSurface);
    rankedPhysicalDevices.insert(std::make_pair(deviceScore, physicalDevice));
  }

  if (rankedPhysicalDevices.rbegin()->first > 0) {
    return rankedPhysicalDevices.rbegin()->second;
  }

  throw std::runtime_error("No suitable device supporting vulkan found");
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

vk::Device VulkanRenderer::createVulkanDevice(vk::PhysicalDevice const &physicalDevice,
                                              QueueFamilyIndices const &queueFamilyIndices) const {

  float queuePriority = 1.0f;
  std::set<uint32_t> uniqueQueueFamilyIndices = {queueFamilyIndices.graphics.value(),
                                                 queueFamilyIndices.transfer.value(),
                                                 queueFamilyIndices.present.value()};

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
  queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());

  for (uint32_t const &queueFamilyIndex : uniqueQueueFamilyIndices) {
    queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{{}, queueFamilyIndex, 1, &queuePriority});
  }

  vk::PhysicalDeviceFeatures enabledDeviceFeatures{};

  vk::DeviceCreateInfo const deviceCreateInfo{
      {}, queueCreateInfos, {}, mRequiredDeviceExtensionsNames, &enabledDeviceFeatures};
  return physicalDevice.createDevice(deviceCreateInfo);
}

vk::Queue VulkanRenderer::retrieveQueue(vk::Device const &device,
                                        QueueFamilyIndices const &queueFamilyIndices,
                                        QueueFamily const &queueFamily) const {

  uint32_t queueFamilyIndex = 0u;
  switch (queueFamily) {
  case QueueFamily::Graphics:
    queueFamilyIndex = queueFamilyIndices.graphics.value();
    break;
  case QueueFamily::Transfer:
    queueFamilyIndex = queueFamilyIndices.transfer.value();
    break;
  case QueueFamily::Present:
    queueFamilyIndex = queueFamilyIndices.present.value();
    break;
  }

  return device.getQueue(queueFamilyIndex, 0);
}
} // namespace flex
