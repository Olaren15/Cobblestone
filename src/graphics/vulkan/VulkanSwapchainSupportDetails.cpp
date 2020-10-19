#include "graphics/vulkan/VulkanSwapchainSupportDetails.hpp"

#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {
VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(
    VulkanSwapchainSupportDetails const &swapchainSupportDetails) {
  capabilities = swapchainSupportDetails.capabilities;
  formats = swapchainSupportDetails.formats;
  presentModes = swapchainSupportDetails.presentModes;
}

flex::VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(
    VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface) {
  validateVkResult(
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));

  uint32_t vectorLength;
  validateVkResult(
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &vectorLength, nullptr));
  formats.resize(vectorLength);
  validateVkResult(
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &vectorLength, formats.data()));

  validateVkResult(
      vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &vectorLength, nullptr));
  presentModes.resize(vectorLength);
  validateVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &vectorLength,
                                                             presentModes.data()));
}

bool VulkanSwapchainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}
} // namespace flex