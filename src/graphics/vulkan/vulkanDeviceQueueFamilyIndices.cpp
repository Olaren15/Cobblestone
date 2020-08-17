#include "graphics/vulkan/vulkanDeviceQueueFamilyIndices.hpp"

namespace flex {
VulkanDeviceQueueFamilyIndices::VulkanDeviceQueueFamilyIndices(vk::PhysicalDevice const &physicalDevice) {
  std::vector<vk::QueueFamilyProperties> deviceQueueFamilyProperties = physicalDevice.getQueueFamilyProperties();

  uint32_t i = 0;
  for (vk::QueueFamilyProperties const &queueFamilyProperty : deviceQueueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics) {
      graphics = i;
    }
    i++;
  }
}
} // namespace flex