#pragma once

#include "vulkan/vulkan.h"

#include "graphics/vulkan/VulkanQueues.hpp"
#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {
struct VulkanFrame {
public:
  VkSemaphore imageAvailableSemaphore{};
  VkSemaphore renderFinishedSemaphore{};
  VkFence renderFinishedFence{};

  VkCommandPool commandPool{};
  VkCommandBuffer commandBuffer{};

  void initialise(VkDevice const& device, VulkanQueues const& queues);
  void destroy(VkDevice const &device);
};
} // namespace flex