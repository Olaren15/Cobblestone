#pragma once

#include "Game/Chunks/Chunk.hpp"

namespace cbl {
struct ChunkGenerator {
private:
  static void
  addSideToChunk(Chunk &chunk, int x, int y, int z, uint32_t &i,
                 std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>> const &sideData);

public:
  [[nodiscard]] static Chunk generate();
};
} // namespace cbl