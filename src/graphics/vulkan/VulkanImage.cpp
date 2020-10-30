#include "graphics/vulkan/VulkanImage.hpp"

#include <stdexcept>

namespace flex {

VkFormat VulkanImage::findSupportedFormat(VulkanGPU const &gpu,
                                          std::vector<VkFormat> const &formatChoices,
                                          VkImageTiling const &requestedTiling,
                                          VkFormatFeatureFlags const &requestedFeatures) {
  for (VkFormat format : formatChoices) {
    VkFormatProperties properties{};
    vkGetPhysicalDeviceFormatProperties(gpu.physicalDevice, format, &properties);

    switch (requestedTiling) {
    case VK_IMAGE_TILING_OPTIMAL:
      if ((properties.optimalTilingFeatures & requestedFeatures) == requestedFeatures) {
        return format;
      }
      break;
    case VK_IMAGE_TILING_LINEAR:
      if ((properties.linearTilingFeatures & requestedFeatures) == requestedFeatures) {
        return format;
      }
      break;
    case VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT:
    case VK_IMAGE_TILING_MAX_ENUM:
      break;
    }
  }

  throw std::runtime_error("Failed to find supported VkFormat");
}

bool VulkanImage::hasStencilComponent(VkFormat format) {
  switch (format) {
  case VK_FORMAT_S8_UINT:
  case VK_FORMAT_D16_UNORM_S8_UINT:
  case VK_FORMAT_D24_UNORM_S8_UINT:
  case VK_FORMAT_D32_SFLOAT_S8_UINT:
    return true;
  default:
    return false;
  }
}

} // namespace flex