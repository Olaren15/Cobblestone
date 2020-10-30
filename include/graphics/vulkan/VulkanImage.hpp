#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/VulkanGPU.hpp"
#include "graphics/vulkan/vk_mem_alloc.h"

namespace flex {
struct VulkanMemoryManager;

struct VulkanImage {
public:
  VmaAllocation allocation{};
  VkImage image{};
  VkImageView imageView{};

  VkFormat format{};
  VkExtent2D extent{};

  static VkFormat findSupportedFormat(VulkanGPU const &gpu,
                                      std::vector<VkFormat> const &formatChoices,
                                      VkImageTiling const &requestedTiling,
                                      VkFormatFeatureFlags const &requestedFeatures);
  static bool hasStencilComponent(VkFormat format);
};
} // namespace flex