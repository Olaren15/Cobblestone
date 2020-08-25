#include "graphics/vulkan/VulkanSwapchainSupportDetails.hpp"

flex::VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(
    VulkanSwapchainSupportDetails const &swapchainSupportDetails) {
  capabilities = swapchainSupportDetails.capabilities;
  formats = swapchainSupportDetails.formats;
  presentModes = swapchainSupportDetails.presentModes;
}

flex::VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(VkPhysicalDevice const &physicalDevice,
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

bool flex::VulkanSwapchainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}
