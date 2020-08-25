#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace flex {
struct VulkanSwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;

  VulkanSwapchainSupportDetails() = default;
  VulkanSwapchainSupportDetails(VulkanSwapchainSupportDetails const &swapChainSupportDetails);
  explicit VulkanSwapchainSupportDetails(VkPhysicalDevice const &physicalDevice,
                                         VkSurfaceKHR const &surface);
  ~VulkanSwapchainSupportDetails() = default;

  [[nodiscard]] bool isUsable() const;
};
} // namespace flex
