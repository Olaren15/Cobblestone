#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

namespace flex {
struct VulkanDeviceQueueFamilyIndices {
  std::optional<uint32_t> graphics;

  VulkanDeviceQueueFamilyIndices() = default;
  VulkanDeviceQueueFamilyIndices(vk::PhysicalDevice const &physicalDevice);
};
} // namespace flex