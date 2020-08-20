#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

namespace flex {
struct QueueFamilyIndices {
  std::optional<uint32_t> graphics;
  std::optional<uint32_t> transfer;
  std::optional<uint32_t> present;

  QueueFamilyIndices() = default;
  explicit QueueFamilyIndices(vk::PhysicalDevice const &physicalDevice,
                              vk::SurfaceKHR const &surface);
  QueueFamilyIndices(QueueFamilyIndices const &queueFamilyIndices);

  [[nodiscard]] bool isComplete() const;
};
} // namespace flex
