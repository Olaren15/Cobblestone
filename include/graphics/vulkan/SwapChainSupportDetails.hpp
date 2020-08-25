#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace flex {
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;

  SwapChainSupportDetails() = default;
  SwapChainSupportDetails(SwapChainSupportDetails const &swapChainSupportDetails);
  explicit SwapChainSupportDetails(VkPhysicalDevice const &physicalDevice,
                                   VkSurfaceKHR const &surface);
  ~SwapChainSupportDetails() = default;

  [[nodiscard]] bool isUsable() const;
};
} // namespace flex
