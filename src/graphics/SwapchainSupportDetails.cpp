#include "graphics/SwapchainSupportDetails.hpp"

#include "graphics/VulkanHelpers.hpp"

namespace flex {
SwapchainSupportDetails::SwapchainSupportDetails(
    SwapchainSupportDetails const &swapchainSupportDetails) {
  capabilities = swapchainSupportDetails.capabilities;
  formats = swapchainSupportDetails.formats;
  presentModes = swapchainSupportDetails.presentModes;
}

flex::SwapchainSupportDetails::SwapchainSupportDetails(GPU const &gpu) {
  validateVkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.physicalDevice, gpu.renderSurface,
                                                             &capabilities));

  uint32_t vectorLength;
  validateVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.physicalDevice, gpu.renderSurface,
                                                        &vectorLength, nullptr));
  formats.resize(vectorLength);
  validateVkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.physicalDevice, gpu.renderSurface,
                                                        &vectorLength, formats.data()));

  validateVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.physicalDevice, gpu.renderSurface,
                                                             &vectorLength, nullptr));
  presentModes.resize(vectorLength);
  validateVkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.physicalDevice, gpu.renderSurface,
                                                             &vectorLength, presentModes.data()));
}

bool SwapchainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}
} // namespace flex