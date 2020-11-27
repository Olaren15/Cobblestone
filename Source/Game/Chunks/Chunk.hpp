#pragma once

#include <array>

#include "Core/World/World.hpp"
#include "Game/Block/Block.hpp"
#include "Graphics/Mesh/Mesh.hpp"

namespace cbl {
struct Chunk {
private:
  void addSideToMesh(int const &x, int const &y, int const &z,
                     std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>> const &sideData);

public:
  static constexpr unsigned int BlocksX = 16;
  static constexpr unsigned int BlocksY = 16;
  static constexpr unsigned int BlocksZ = 16;

  std::array<std::array<std::array<Block::Type, BlocksZ>, BlocksY>, BlocksX> blocks{
      Block::Type::eAir};
  gfx::Mesh mesh{{}, {}};
  glm::vec3 position{0};

  Chunk *neighbourXPlus{nullptr};
  Chunk *neighbourXMinus{nullptr};
  Chunk *neighbourZPlus{nullptr};
  Chunk *neighbourZMinus{nullptr};

  void rebuildMesh();
};
} // namespace cbl