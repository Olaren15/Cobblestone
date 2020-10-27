#include "graphics/vulkan/VulkanImage.hpp"

#include <stdexcept>

#include "graphics/vulkan/VulkanMemoryManager.hpp"

namespace flex {
VulkanImage::VulkanImage(VulkanMemoryManager &manager) : memoryManager(manager) {}

VulkanImage &VulkanImage::operator=(VulkanImage other) {
  allocation = other.allocation;
  image = other.image;
  imageView = other.imageView;
  memoryManager = other.memoryManager;
  return *this;
}

VkFormat VulkanImage::findSupportedFormat(VkPhysicalDevice const &physicalDevice,
                                          std::vector<VkFormat> const &formatChoices,
                                          VkImageTiling const &requestedTiling,
                                          VkFormatFeatureFlags const &requestedFeatures) {
  for (VkFormat format : formatChoices) {
    VkFormatProperties properties{};
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

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

VkFormat VulkanImage::getDepthBufferFormat(VkPhysicalDevice const &physicalDevice) {
  std::vector<VkFormat> const preferredFormats{
      VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT};

  return findSupportedFormat(physicalDevice, preferredFormats, VK_IMAGE_TILING_OPTIMAL,
                             VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
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