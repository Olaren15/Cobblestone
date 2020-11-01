#include "graphics/vulkan/VulkanGPU.hpp"

#include <map>
#include <stdexcept>

#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {

void VulkanGPU::createInstance(RenderWindow const &renderWindow) {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Flex Engine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Flex Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  std::vector<char const *> enabledExtensions = renderWindow.getRequiredVulkanExtensions();
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

  validateVkResult(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
}

void VulkanGPU::selectPhysicalDevice() {
  uint32_t physicalDeviceCount;
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
  std::vector<VkPhysicalDevice> availablePhysicalDevices{physicalDeviceCount};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, availablePhysicalDevices.data());

  std::multimap<int, VkPhysicalDevice> rankedPhysicalDevices = {};

  for (VkPhysicalDevice const &availablePhysicalDevice : availablePhysicalDevices) {
    unsigned int deviceScore =
        ratePhysicalDevice(availablePhysicalDevice, renderSurface, mRequiredDeviceExtensionsNames);
    rankedPhysicalDevices.insert(std::make_pair(deviceScore, availablePhysicalDevice));
  }

  if (rankedPhysicalDevices.rbegin()->first > 0) {
    physicalDevice = rankedPhysicalDevices.rbegin()->second;
  } else {
    throw std::runtime_error("No suitable device supporting vulkan found");
  }
}

void VulkanGPU::createDevice() {
  float queuePriority = 1.0f;
  std::set<uint32_t> uniqueQueueFamilyIndices = queueFamilyIndices.getUniqueIndices();

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

  validateVkResult(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));
}

void VulkanGPU::retrieveQueues() {
  vkGetDeviceQueue(device, queueFamilyIndices.graphics.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, queueFamilyIndices.transfer.value(), 0, &transferQueue);
  vkGetDeviceQueue(device, queueFamilyIndices.present.value(), 0, &presentQueue);
}

void VulkanGPU::initialise(RenderWindow const &renderWindow) {
  createInstance(renderWindow);
  renderSurface = renderWindow.getDrawableVulkanSurface(instance);
  selectPhysicalDevice();
  queueFamilyIndices = VulkanQueueFamilyIndices{*this};
  createDevice();
  retrieveQueues();
}

void VulkanGPU::waitIdle() const { validateVkResult(vkDeviceWaitIdle(device)); }

void VulkanGPU::destroy() const {
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, renderSurface, nullptr);
  vkDestroyInstance(instance, nullptr);
}

} // namespace flex