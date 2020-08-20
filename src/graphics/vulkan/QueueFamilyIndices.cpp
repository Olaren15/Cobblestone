#include "graphics/vulkan/QueueFamilyIndices.hpp"

namespace flex {
QueueFamilyIndices::QueueFamilyIndices(
    vk::PhysicalDevice const &physicalDevice) {
  std::vector<vk::QueueFamilyProperties> deviceQueueFamilyProperties =
      physicalDevice.getQueueFamilyProperties();

  uint32_t i = 0;
  for (vk::QueueFamilyProperties const &queueFamilyProperty :
       deviceQueueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics) {
      graphics = i;
    }
    i++;
  }
}

QueueFamilyIndices::QueueFamilyIndices(
    QueueFamilyIndices const &queueFamilyIndices) {
  graphics = queueFamilyIndices.graphics;
  transfer = queueFamilyIndices.transfer;
}

} // namespace flex
