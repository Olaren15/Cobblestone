#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "GPU.hpp"
#include "vk_mem_alloc.h"

namespace flex {
struct MemoryManager;

struct Image {
public:
  VmaAllocation allocation{};
  VkImage image{};
  VkImageView imageView{};

  VkFormat format{};
  VkExtent2D extent{};

  static VkFormat findSupportedFormat(GPU const &gpu,
                                      std::vector<VkFormat> const &formatChoices,
                                      VkImageTiling const &requestedTiling,
                                      VkFormatFeatureFlags const &requestedFeatures);
  static bool hasStencilComponent(VkFormat format);
};
} // namespace flex