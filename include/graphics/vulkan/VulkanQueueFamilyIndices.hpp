#pragma once

#include <optional>
#include <set>

#include <vulkan/vulkan.h>

namespace flex {
struct VulkanGPU;
struct VulkanQueueFamilyIndices {
  std::optional<uint32_t> graphics;
  std::optional<uint32_t> transfer;
  std::optional<uint32_t> present;

  VulkanQueueFamilyIndices() = default;
  VulkanQueueFamilyIndices(VulkanQueueFamilyIndices const &queueFamilyIndices);
  explicit VulkanQueueFamilyIndices(VulkanGPU const &gpu);
  ~VulkanQueueFamilyIndices() = default;

  [[nodiscard]] bool isComplete() const;
  [[nodiscard]] bool hasUniqueTransferQueue() const;
  [[nodiscard]] std::set<uint32_t> getUniqueIndices() const;
};
} // namespace flex
