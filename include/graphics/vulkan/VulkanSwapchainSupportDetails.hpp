#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/VulkanGPU.hpp"

namespace flex {
struct VulkanSwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;

  VulkanSwapchainSupportDetails() = default;
  VulkanSwapchainSupportDetails(VulkanSwapchainSupportDetails const &swapChainSupportDetails);
  explicit VulkanSwapchainSupportDetails(VulkanGPU const &gpu);
  ~VulkanSwapchainSupportDetails() = default;

  [[nodiscard]] bool isUsable() const;
};
} // namespace flex
