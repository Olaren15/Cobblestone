#pragma once

#include "vulkan/vulkan.h"

#include "Graphics/GPU/GPU.hpp"

namespace cbl::gfx {
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