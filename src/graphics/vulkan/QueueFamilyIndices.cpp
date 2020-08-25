#include "graphics/vulkan/QueueFamilyIndices.hpp"

namespace flex {

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
  for (VkQueueFamilyProperties const &queueFamilyProperty : queueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics = i;
    }

    if (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      transfer = i;
    }

    VkBool32 surfaceSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &surfaceSupported);
    if (surfaceSupported == VK_TRUE) {
      present = i;
    }

    i++;
  }
}

bool QueueFamilyIndices::isComplete() const {
  return graphics.has_value() && transfer.has_value() && present.has_value();
}

} // namespace flex
