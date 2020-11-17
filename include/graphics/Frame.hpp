#pragma once

#include "vulkan/vulkan.h"

#include "GPU.hpp"
#include "VulkanHelpers.hpp"

namespace flex {
struct Frame {
public:
  VkSemaphore imageAvailableSemaphore{};
  VkSemaphore renderFinishedSemaphore{};
  VkFence renderFinishedFence{};

  VkCommandPool commandPool{};
  VkCommandBuffer commandBuffer{};

  void initialise(GPU const &gpu);
  void destroy(GPU const &gpu) const;
};
} // namespace flex