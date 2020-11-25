#pragma once

#include "Game/Chunks/Chunk.hpp"

namespace cbl {
struct ChunkGenerator {
private:
  static void
  addSideToChunk(Chunk &chunk, int const &x, int const &y, int const &z, uint32_t &i,
                 std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>> const &sideData);

public:
  [[nodiscard]] static Chunk generate();
  [[nodiscard]] static std::vector<Chunk> generateMany(int const &numX, int const &numZ);
};
} // namespace cbl