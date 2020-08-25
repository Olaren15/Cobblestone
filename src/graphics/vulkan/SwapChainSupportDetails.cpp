#include "graphics/vulkan/SwapchainSupportDetails.hpp"

flex::SwapchainSupportDetails::SwapchainSupportDetails(
    SwapchainSupportDetails const &swapchainSupportDetails) {
  capabilities = swapchainSupportDetails.capabilities;
  formats = swapchainSupportDetails.formats;
  presentModes = swapchainSupportDetails.presentModes;
}

flex::SwapchainSupportDetails::SwapchainSupportDetails(VkPhysicalDevice const &physicalDevice,
                                                       VkSurfaceKHR const &surface) {
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

  uint32_t vectorLength;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &vectorLength, nullptr);
  formats.resize(vectorLength);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &vectorLength, formats.data());

  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &vectorLength, nullptr);
  presentModes.resize(vectorLength);
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &vectorLength,
                                            presentModes.data());
}

bool flex::SwapchainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}
