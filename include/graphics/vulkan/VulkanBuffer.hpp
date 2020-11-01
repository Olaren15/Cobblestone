#pragma once

#include <vulkan/vulkan.h>

#include "graphics/vulkan/vk_mem_alloc.h"

namespace flex {
struct VulkanMemoryManager;

struct VulkanBuffer {
  bool isValid = false;
  VmaAllocation allocation{};
  VkBuffer buffer{};
  VkDeviceSize size{};
  VulkanMemoryManager *memoryManager{};
};
} // namespace flex
