#pragma once

#include <optional>

#include <vulkan/vulkan.h>

namespace flex {
struct VulkanQueueFamilyIndices {
  std::optional<uint32_t> graphics;
  std::optional<uint32_t> transfer;
  std::optional<uint32_t> present;

  VulkanQueueFamilyIndices() = default;
  VulkanQueueFamilyIndices(VulkanQueueFamilyIndices const &queueFamilyIndices);
  explicit VulkanQueueFamilyIndices(VkPhysicalDevice const &physicalDevice,
                                    VkSurfaceKHR const &surface);
  ~VulkanQueueFamilyIndices() = default;

  [[nodiscard]] bool isComplete() const;
};
} // namespace flex
