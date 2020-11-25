#pragma once

#include <array>

#include "Game/Block/Block.hpp"
#include "Graphics/Mesh/Mesh.hpp"

namespace cbl {
struct Chunk {
  static constexpr unsigned int BlocksX = 16;
  static constexpr unsigned int BlocksY = 16;
  static constexpr unsigned int BlocksZ = 16;

  std::array<std::array<std::array<Block::Type, BlocksZ>, BlocksY>, BlocksX> blocks{
      Block::Type::eAir};
  gfx::Mesh mesh{{}, {}};
  glm::vec3 position{0};
};
} // namespace cbl