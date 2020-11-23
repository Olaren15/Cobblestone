#pragma once

#include <vector>

#include "Graphics/Memory/Buffer/Buffer.hpp"
#include "Graphics/Vertex/Vertex.hpp"

namespace cbl::gfx {
struct Mesh {
  explicit Mesh(std::vector<uint32_t> const &indices, std::vector<Vertex> const &vertices);

  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;
  glm::mat4 position{1};
  mem::Buffer buffer;

  [[nodiscard]] size_t getIndicesSize() const;
  [[nodiscard]] size_t getVerticesSize() const;
  [[nodiscard]] size_t getRequiredBufferSize() const;
};
} // namespace flex