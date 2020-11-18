#include "graphics/QueueFamilyIndices.hpp"

#include <vector>

#include "graphics/GPU.hpp"

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
    if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphics.has_value()) {
      graphics = i;
    }

    // look for transfer-only queue
    if (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT &&
        !(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT &&
          queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
        !transfer.has_value()) {
      transfer = i;
    }

    VkBool32 surfaceSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &surfaceSupported);
    if (surfaceSupported == VK_TRUE && !present.has_value()) {
      present = i;
    }

    i++;
  }

  // did not find transfer-only queue
  if (!transfer.has_value()) {
    // graphics queues always support transfer
    transfer = graphics;
  }
}

bool QueueFamilyIndices::isComplete() const {
  return graphics.has_value() && transfer.has_value() && present.has_value();
}

bool QueueFamilyIndices::hasUniqueTransferQueue() const {
  return graphics.value() != transfer.value();
}

std::set<uint32_t> QueueFamilyIndices::getUniqueIndices() const {
  return std::set<uint32_t>{graphics.value(), transfer.value(), present.value()};
}

} // namespace flex
