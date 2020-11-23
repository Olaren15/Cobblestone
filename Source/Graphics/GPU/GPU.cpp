#include "GPU.hpp"

#include <map>
#include <stdexcept>

#include "Graphics/Swapchain/Swapchain.hpp"
#include "Graphics/Utils/VulkanHelpers.hpp"

namespace cbl::gfx {

QueueFamilyIndices::QueueFamilyIndices(QueueFamilyIndices const &queueFamilyIndices) {
  graphics = queueFamilyIndices.graphics;
  transfer = queueFamilyIndices.transfer;
  present = queueFamilyIndices.present;
}

QueueFamilyIndices::QueueFamilyIndices(VkPhysicalDevice const &physicalDevice,
                                       VkSurfaceKHR const &surface) {
  uint32_t propertiesCount;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertiesCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties{propertiesCount};
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertiesCount,
                                           queueFamilyProperties.data());

  uint32_t i = 0;
  bool transferFound, graphicsFound, presentFound;

  for (VkQueueFamilyProperties const &queueFamilyProperty : queueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphicsFound) {
      graphics = i;
    }

    // look for transfer-only queue
    if (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT &&
        !(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT &&
          queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
        !transferFound) {
      transfer = i;
    }

    VkBool32 surfaceSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &surfaceSupported);
    if (surfaceSupported == VK_TRUE && !presentFound) {
      present = i;
    }

    i++;
  }

  if (!transferFound) {
    // graphics queues always support transfer
    transfer = graphics;
  }
}

std::set<uint32_t> QueueFamilyIndices::getUniqueIndices() const {
  return std::set<uint32_t>{graphics, transfer, present};
}

GPU::GPU(Window const &window) {
  createInstance(window);
  renderSurface = window.getDrawableVulkanSurface(instance);
  selectPhysicalDevice();
  queueFamilyIndices = QueueFamilyIndices{physicalDevice, renderSurface};
  createDevice();
  retrieveQueues();
}

GPU::~GPU() {
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, renderSurface, nullptr);
  vkDestroyInstance(instance, nullptr);
}

void GPU::createInstance(Window const &renderWindow) {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Cobblestone";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Cobblestone";
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

void GPU::selectPhysicalDevice() {
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

void GPU::createDevice() {
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
  enabledDeviceFeatures.samplerAnisotropy = VK_TRUE;

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

void GPU::retrieveQueues() {
  vkGetDeviceQueue(device, queueFamilyIndices.graphics, 0, &graphicsQueue);
  vkGetDeviceQueue(device, queueFamilyIndices.transfer, 0, &transferQueue);
  vkGetDeviceQueue(device, queueFamilyIndices.present, 0, &presentQueue);
}

unsigned int GPU::ratePhysicalDevice(VkPhysicalDevice const &physicalDevice,
                                     VkSurfaceKHR const &vulkanSurface,
                                     std::vector<char const *> const &requiredExtensions) {
  unsigned int score = 1u;

  VkPhysicalDeviceProperties physicalDeviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
  if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000u;
  }

  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
  if (!physicalDeviceFeatures.samplerAnisotropy) {
    return 0u;
  }

  if (!physicalDeviceSupportsExtensions(physicalDevice, requiredExtensions)) {
    return 0u;
  }

  if (SwapchainSupportDetails const swapchainSupportDetails{physicalDevice, vulkanSurface};
      !swapchainSupportDetails.isUsable()) {
    return 0u;
  }

  return score;
}

bool GPU::physicalDeviceSupportsExtensions(VkPhysicalDevice const &physicalDevice,
                                           std::vector<const char *> const &extensions) {

  uint32_t availableExtensionsCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions{availableExtensionsCount};
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount,
                                       availableExtensions.data());

  for (std::string const &requiredExtensionName : extensions) {
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

void GPU::waitIdle() const { validateVkResult(vkDeviceWaitIdle(device)); }

bool GPU::isDedicated() const {
  VkPhysicalDeviceProperties deviceProperties{};
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

  return deviceProperties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

} // namespace cbl::gfx