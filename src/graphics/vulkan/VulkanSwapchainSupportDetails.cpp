#include "graphics/vulkan/VulkanSwapchainSupportDetails.hpp"

#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {
VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(
    VulkanSwapchainSupportDetails const &swapchainSupportDetails) {
  capabilities = swapchainSupportDetails.capabilities;
  formats = swapchainSupportDetails.formats;
  presentModes = swapchainSupportDetails.presentModes;
}

flex::VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(VulkanGPU const &gpu) {
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

bool VulkanSwapchainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}
} // namespace flex