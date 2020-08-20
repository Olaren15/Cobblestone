#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace flex {
struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;

  SwapChainSupportDetails() = default;
  SwapChainSupportDetails(SwapChainSupportDetails const &swapChainSupportDetails);
  explicit SwapChainSupportDetails(vk::PhysicalDevice const &physicalDevice,
                                   vk::SurfaceKHR const &surface);
  ~SwapChainSupportDetails() = default;

  [[nodiscard]] bool isUsable() const;
};
} // namespace flex
