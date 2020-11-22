#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "External/vk_mem_alloc/vk_mem_alloc.h"
#include "Graphics/GPU/GPU.hpp"

namespace cbl::gfx::mem {
struct MemoryManager;

struct Image {
public:
  VmaAllocation allocation{};
  VkImage image{};
  VkImageView imageView{};

  VkFormat format{};
  VkExtent2D extent{};

  static VkFormat findSupportedFormat(GPU const &gpu, std::vector<VkFormat> const &formatChoices,
                                      VkImageTiling const &requestedTiling,
                                      VkFormatFeatureFlags const &requestedFeatures);
  static bool hasStencilComponent(VkFormat format);
};
} // namespace flex