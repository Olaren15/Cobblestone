#include "graphics/vulkan/SwapChainSupportDetails.hpp"

flex::SwapChainSupportDetails::SwapChainSupportDetails(
    SwapChainSupportDetails const &swapChainSupportDetails) {
  capabilities = swapChainSupportDetails.capabilities;
  formats = swapChainSupportDetails.formats;
  presentModes = swapChainSupportDetails.presentModes;
}

flex::SwapChainSupportDetails::SwapChainSupportDetails(VkPhysicalDevice const &physicalDevice,
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

bool flex::SwapChainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}
