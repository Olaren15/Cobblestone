#pragma once

#include "vulkan/vulkan.h"

#include "GPU.hpp"
#include "VulkanHelpers.hpp"

namespace flex {
struct Frame {
  GPU const &mGPU;

public:
  VkSemaphore imageAvailableSemaphore{};
  VkSemaphore renderFinishedSemaphore{};
  VkFence renderFinishedFence{};

  VkCommandPool commandPool{};
  VkCommandBuffer commandBuffer{};

  Frame() = delete;
  explicit Frame(GPU const &gpu);
  ~Frame();
};
} // namespace flex