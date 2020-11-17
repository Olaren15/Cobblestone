#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace flex {
struct MemoryManager;

struct Buffer {
  bool isValid = false;
  VmaAllocation allocation{};
  VkBuffer buffer{};
  VkDeviceSize size{};
  MemoryManager *memoryManager{};
};
} // namespace flex
