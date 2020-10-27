#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/vk_mem_alloc.h"

namespace flex {
struct VulkanMemoryManager;

struct VulkanImage {
private:
  static VkFormat findSupportedFormat(VkPhysicalDevice const& physicalDevice, std::vector<VkFormat> const
      &formatChoices,
                                      VkImageTiling const &requestedTiling,
                                      VkFormatFeatureFlags const &requestedFeatures);

public:
  VmaAllocation allocation{};
  VkImage image{};
  VkImageView imageView{};
  VulkanMemoryManager &memoryManager;

  explicit VulkanImage(VulkanMemoryManager &manager);
  VulkanImage &operator=(VulkanImage other);

  static VkFormat getDepthBufferFormat(VkPhysicalDevice const &physicalDevice);
  static bool hasStencilComponent(VkFormat format);
};
} // namespace flex