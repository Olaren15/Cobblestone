#pragma once

#include <optional>
#include <set>

#include <vulkan/vulkan.h>

namespace flex {
struct GPU;
struct QueueFamiliIndices {
  std::optional<uint32_t> graphics;
  std::optional<uint32_t> transfer;
  std::optional<uint32_t> present;

  QueueFamiliIndices() = default;
  QueueFamiliIndices(QueueFamiliIndices const &queueFamilyIndices);
  explicit QueueFamiliIndices(GPU const &gpu);
  ~QueueFamiliIndices() = default;

  [[nodiscard]] bool isComplete() const;
  [[nodiscard]] bool hasUniqueTransferQueue() const;
  [[nodiscard]] std::set<uint32_t> getUniqueIndices() const;
};
} // namespace flex
