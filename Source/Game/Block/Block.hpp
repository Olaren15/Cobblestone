#pragma once

#include <algorithm>
#include <vector>

#include "Graphics/Vertex/Vertex.hpp"

namespace cbl {
struct Block {
  enum class Type { eAir, eGrass };

  [[nodiscard]] static std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
  getFrontVertices(Type const &type);
  [[nodiscard]] static std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
  getRightVertices(Type const &type);
  [[nodiscard]] static std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
  getBackVertices(Type const &type);
  [[nodiscard]] static std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
  getLeftVertices(Type const &type);
  [[nodiscard]] static std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
  getTopVertices(Type const &type);
  [[nodiscard]] static std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
  getBottomVertices(Type const &type);
};
} // namespace cbl