#pragma once

#include "vulkan/vulkan.h"

#include "graphics/vulkan/VulkanGPU.hpp"
#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {
struct VulkanFrame {
public:
  VkSemaphore imageAvailableSemaphore{};
  VkSemaphore renderFinishedSemaphore{};
  VkFence renderFinishedFence{};

  VkCommandPool commandPool{};
  VkCommandBuffer commandBuffer{};

  void initialise(VulkanGPU const &gpu);
  void destroy(VulkanGPU const &gpu) const;
};
} // namespace flex