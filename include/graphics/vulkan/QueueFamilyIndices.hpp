#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

namespace flex {
struct QueueFamilyIndices {
  std::optional<uint32_t> graphics;
  std::optional<uint32_t> transfer;

  QueueFamilyIndices() = default;
  QueueFamilyIndices(vk::PhysicalDevice const &physicalDevice);
  QueueFamilyIndices(QueueFamilyIndices const &queueFamilyIndices);
};
} // namespace flex
