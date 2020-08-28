#pragma once

#include <vector>

#include "graphics/Vertex.hpp"

namespace flex {
struct Mesh {
  std::vector<uint32_t> indices;
  std::vector<Vertex> vertices;

  [[nodiscard]] size_t getIndicesSize() const;
  [[nodiscard]] size_t getVerticesSize() const;
};
} // namespace flex