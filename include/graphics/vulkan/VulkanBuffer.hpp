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

  void allocateVertex(VmaAllocator const &allocator, size_t const &bufferSize);
  void setContent(VmaAllocator const &allocator, void *data) const;
  void destroy(VmaAllocator const &allocator) const;
};
} // namespace flex
