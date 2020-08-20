#include "graphics/vulkan/QueueFamilyIndices.hpp"

namespace flex {

QueueFamilyIndices::QueueFamilyIndices(QueueFamilyIndices const &queueFamilyIndices) {
  graphics = queueFamilyIndices.graphics;
  transfer = queueFamilyIndices.transfer;
  present = queueFamilyIndices.present;
}

QueueFamilyIndices::QueueFamilyIndices(vk::PhysicalDevice const &physicalDevice,
                                       vk::SurfaceKHR const &surface) {
  std::vector<vk::QueueFamilyProperties> deviceQueueFamilyProperties = physicalDevice.
      getQueueFamilyProperties();

  uint32_t i = 0;
  for (vk::QueueFamilyProperties const &queueFamilyProperty : deviceQueueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics) {
      graphics = i;
    }

    if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer) {
      transfer = i;
    }

    if (physicalDevice.getSurfaceSupportKHR(i, surface) == VK_TRUE) {
      present = i;
    }

    i++;
  }
}

bool QueueFamilyIndices::isComplete() const {
  return graphics.has_value() && transfer.has_value() && present.has_value();
}

} // namespace flex
