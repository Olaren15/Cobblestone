#pragma once

#include "Game/Chunks/Chunk.hpp"

namespace cbl {
struct ChunkGenerator {
private:
public:
  [[nodiscard]] static Chunk generate(int const &posX, int const &posZ);
  [[nodiscard]] static std::map<std::pair<int, int>, Chunk> generateMany(int const &numX,
                                                                         int const &numZ);
};
} // namespace cbl