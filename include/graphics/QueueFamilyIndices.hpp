#pragma once

#include <optional>
#include <set>

#include <vulkan/vulkan.h>

namespace flex {
struct GPU;
struct QueueFamilyIndices {
  std::optional<uint32_t> graphics;
  std::optional<uint32_t> transfer;
  std::optional<uint32_t> present;

  QueueFamilyIndices() = default;
  QueueFamilyIndices(QueueFamilyIndices const &queueFamilyIndices);
  explicit QueueFamilyIndices(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface);
  ~QueueFamilyIndices() = default;

  [[nodiscard]] bool isComplete() const;
  [[nodiscard]] bool hasUniqueTransferQueue() const;
  [[nodiscard]] std::set<uint32_t> getUniqueIndices() const;
};
} // namespace flex
