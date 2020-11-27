#pragma once

#include <algorithm>
#include <vector>

#include "Graphics/Vertex/Vertex.hpp"

namespace cbl {
struct Chunk;

struct Block {
public:
  enum class Type { eAir, eGrass, eDirt };
  enum class Side { eFront, eRight, eBack, eLeft, eTop, eBottom };

  [[nodiscard]] static std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
  getVertices(Side const &side, Type const &type);
};
} // namespace cbl