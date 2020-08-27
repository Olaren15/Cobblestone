#include "graphics/vulkan/VulkanQueues.hpp"

namespace flex {

VulkanQueues::VulkanQueues(VulkanQueues const &vulkanQueues) {
  familyIndices = vulkanQueues.familyIndices;
  graphics = vulkanQueues.graphics;
  transfer = vulkanQueues.transfer;
  present = vulkanQueues.present;
}

void VulkanQueues::buildQueueFamilyIndices(VkPhysicalDevice const &physicalDevice,
                                           VkSurfaceKHR const &surface) {
  familyIndices = VulkanQueueFamilyIndices{physicalDevice, surface};
}

void VulkanQueues::retrieveQueues(VkDevice const &device) {
  vkGetDeviceQueue(device, familyIndices.graphics.value(), 0, &graphics);
  vkGetDeviceQueue(device, familyIndices.transfer.value(), 0, &transfer);
  vkGetDeviceQueue(device, familyIndices.present.value(), 0, &present);
}

} // namespace flex
