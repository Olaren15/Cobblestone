#pragma once

#include <vulkan/vulkan.h>

#include "graphics/vulkan/vk_mem_alloc.h"

namespace flex {
struct VulkanMemoryManager;

struct VulkanBuffer {
  VmaAllocation allocation{};
  VkBuffer buffer{};
  VulkanMemoryManager &memoryManager;

  VulkanBuffer() = delete;
  explicit VulkanBuffer(VulkanMemoryManager &manager);
  VulkanBuffer &operator=(VulkanBuffer other);
};
} // namespace flex
