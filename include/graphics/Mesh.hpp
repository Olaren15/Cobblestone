#pragma once

#include <optional>
#include <vector>

#include "graphics/Vertex.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"

namespace flex {
struct Mesh {
private:
  std::vector<uint32_t> mIndices;
  std::vector<Vertex> mVertices;
  std::optional<VulkanBuffer> mVulkanBuffer;

  void updateBufferData();

public:
  explicit Mesh(std::vector<uint32_t> const &indices, std::vector<Vertex> const &vertices);

  void setIndices(std::vector<uint32_t> const &indices);
  void setVertices(std::vector<Vertex> const &vertices);
  void setVulkanBuffer(VulkanBuffer buffer);

  [[nodiscard]] std::vector<uint32_t> getIndices() const;
  [[nodiscard]] std::vector<Vertex> getVertices() const;
  [[nodiscard]] std::optional<VulkanBuffer> getVulkanBuffer() const;

  [[nodiscard]] size_t getIndicesSize() const;
  [[nodiscard]] size_t getVerticesSize() const;
  [[nodiscard]] size_t getRequiredBufferSize() const;
};
} // namespace flex