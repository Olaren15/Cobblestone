#include "graphics/SwapchainSupportDetails.hpp"

#include "graphics/VulkanHelpers.hpp"

namespace flex {
SwapchainSupportDetails::SwapchainSupportDetails(
    SwapchainSupportDetails const &swapchainSupportDetails) {
  capabilities = swapchainSupportDetails.capabilities;
  formats = swapchainSupportDetails.formats;
  presentModes = swapchainSupportDetails.presentModes;
}

flex::SwapchainSupportDetails::SwapchainSupportDetails(VkPhysicalDevice const &physicalDevice,
                                                       VkSurfaceKHR const &surface) {
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

bool SwapchainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}
} // namespace flex