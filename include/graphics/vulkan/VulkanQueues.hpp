#pragma once

#include "graphics/vulkan/VulkanQueueFamilyIndices.hpp"

namespace flex {
struct VulkanQueues {
  VulkanQueueFamilyIndices familyIndices;

  VkQueue graphics{};
  VkQueue transfer{};
  VkQueue present{};

  VulkanQueues() = default;
  VulkanQueues(VulkanQueues const &vulkanQueues);

  void buildQueueFamilyIndices(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface);
  void retrieveQueues(VkDevice const &device);
};
} // namespace flex