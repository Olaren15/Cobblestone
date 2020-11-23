#include "Image.hpp"

#include <stdexcept>

namespace cbl::gfx::mem {

VkFormat Image::findSupportedFormat(GPU const &gpu,
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

} // namespace flex