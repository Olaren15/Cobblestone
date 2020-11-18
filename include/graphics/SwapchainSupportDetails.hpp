#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "GPU.hpp"

namespace flex {
struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;

  SwapchainSupportDetails() = default;
  SwapchainSupportDetails(SwapchainSupportDetails const &swapChainSupportDetails);
  explicit SwapchainSupportDetails(VkPhysicalDevice const &physicalDevice,
                                   VkSurfaceKHR const &surface);
  ~SwapchainSupportDetails() = default;

  [[nodiscard]] bool isUsable() const;
};
} // namespace flex
