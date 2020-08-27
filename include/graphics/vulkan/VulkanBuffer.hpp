#pragma once

#include <vulkan/vulkan.h>

#include "graphics/vulkan/vk_mem_alloc.h"

namespace flex {
struct VulkanBuffer {
  VmaAllocation allocation{};
  VmaAllocationInfo allocationInfo{};
  VkBuffer buffer{};

  VulkanBuffer() = default;
  VulkanBuffer(VulkanBuffer const &) = delete;
};
} // namespace flex
