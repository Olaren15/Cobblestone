#pragma once

#include <vulkan/vulkan.h>

#include "External/vk_mem_alloc/vk_mem_alloc.h"

namespace cbl::gfx::mem {
struct MemoryManager;

struct Buffer {
  bool isValid = false;
  VmaAllocation allocation{};
  VkBuffer buffer{};
  VkDeviceSize size{};
  MemoryManager *memoryManager{};
};
} // namespace flex
