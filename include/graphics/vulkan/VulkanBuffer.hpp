#pragma once

#include <vulkan/vulkan.h>

#include "graphics/vulkan/vk_mem_alloc.h"

namespace flex {
struct VulkanBuffer {
  VmaAllocation allocation{};
  VkBuffer buffer{};
};
} // namespace flex
