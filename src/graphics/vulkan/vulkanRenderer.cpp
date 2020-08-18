#include "graphics/vulkan/vulkanRenderer.hpp"

#include <iostream>
#include <map>
#include <vector>

#include "graphics/renderAPI.hpp"

namespace flex {
VulkanRenderer::VulkanRenderer(Window const &window) {
  if (window.getRenderAPI() != RenderAPI::Vulkan) {
    throw std::runtime_error("Can't create vulkan renderer if window is not "
                             "initialized with the Vulkan render API");
  }

  mVulkanInstance = createVulkanInstance(window);
  mPhysicalDevice = selectPhysicalDevice(mVulkanInstance);
  mQueueFamilyIndices = {mPhysicalDevice};
}

VulkanRenderer::~VulkanRenderer() { mVulkanInstance.destroy(); }

vk::Instance VulkanRenderer::createVulkanInstance(Window const &window) const {
  vk::ApplicationInfo appInfo{window.getTitle().c_str(), VK_MAKE_VERSION(1, 0, 0), "Flex Engine",
                              VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_0};

  std::vector<char const *> enabledExtensions = window.getRequiredVulkanExtensions();

  std::vector<char const *> enabledLayers{};

  if (mEnableValidationLayers) {
    enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
  }

  const vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, enabledLayers, enabledExtensions);

  return createInstance(instanceCreateInfo);
}

vk::PhysicalDevice VulkanRenderer::selectPhysicalDevice(vk::Instance const &vulkanInstance) {
  std::vector<vk::PhysicalDevice> availablePhysicalDevices = vulkanInstance.enumeratePhysicalDevices();

  std::multimap<int, vk::PhysicalDevice> rankedPhysicalDevices = {};

  for (vk::PhysicalDevice const &physicalDevice : availablePhysicalDevices) {
    unsigned int deviceScore = ratePhysicalDevice(physicalDevice);
    rankedPhysicalDevices.insert(std::make_pair(deviceScore, physicalDevice));
  }

  if (rankedPhysicalDevices.rbegin()->first > 0) {
    return rankedPhysicalDevices.rbegin()->second;
  }
  throw std::runtime_error("No suitable device supporting vulkan found");
}

unsigned int VulkanRenderer::ratePhysicalDevice(vk::PhysicalDevice const &physicalDevice) {
  unsigned int score = 0u;

  const vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();

  if (physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
    score += 1000u;
  }

  if (const VulkanDeviceQueueFamilyIndices queueFamilyIndices = {physicalDevice};
      !queueFamilyIndices.graphics.has_value()) {
    score = 0u;
  }

  // TODO: add more checks

  return score;
}
} // namespace flex