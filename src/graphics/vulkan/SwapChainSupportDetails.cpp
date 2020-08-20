#include "graphics/vulkan/SwapChainSupportDetails.hpp"

flex::SwapChainSupportDetails::SwapChainSupportDetails(
    SwapChainSupportDetails const &swapChainSupportDetails) {
  capabilities = swapChainSupportDetails.capabilities;
  formats = swapChainSupportDetails.formats;
  presentModes = swapChainSupportDetails.presentModes;
}

flex::SwapChainSupportDetails::SwapChainSupportDetails(vk::PhysicalDevice const &physicalDevice,
                                                       vk::SurfaceKHR const &surface) {
  capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
  formats = physicalDevice.getSurfaceFormatsKHR(surface);
  presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
}

bool flex::SwapChainSupportDetails::isUsable() const {
  return !(formats.empty() || presentModes.empty());
}
