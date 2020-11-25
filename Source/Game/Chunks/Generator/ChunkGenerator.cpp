#include "ChunkGenerator.hpp"

#include <random>

#include <glm/gtc/matrix_transform.hpp>

namespace cbl {

void ChunkGenerator::addSideToChunk(
    Chunk &chunk, int const &x, int const &y, int const &z, uint32_t &i,
    std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>> const &sideData) {

  for (uint32_t const &index : sideData.first) {
    chunk.mesh.indices.push_back(index + i);
  }

  i += sideData.second.size();

  for (gfx::Vertex const &vertex : sideData.second) {
    chunk.mesh.vertices.push_back(gfx::Vertex{{vertex.position.x + static_cast<float>(x),
                                               vertex.position.y + static_cast<float>(y),
                                               vertex.position.z + static_cast<float>(z)},
                                              {vertex.uvw}});
  }
}

Chunk ChunkGenerator::generate() {
  Chunk chunk{};

  std::default_random_engine random;

  for (auto &x : chunk.blocks) {
    for (auto &y : x) {
      for (auto &z : y) {
        if (random() % 2) {
          z = Block::Type::eGrass;
        } else {
          z = Block::Type::eAir;
        }
      }
    }
  }

  uint32_t i = 0;

  for (int x = 0; x < Chunk::BlocksX; x++) {
    for (int y = 0; y < Chunk::BlocksY; y++) {
      for (int z = 0; z < Chunk::BlocksZ; z++) {
        if (x == 0 || chunk.blocks[x - 1][y][z] == Block::Type::eAir) {
          addSideToChunk(chunk, x, y, z, i, Block::getVertices(Block::Side::eLeft, chunk, x, y, z));
        }

        if (x == Chunk::BlocksX - 1 || chunk.blocks[x + 1][y][z] == Block::Type::eAir) {
          addSideToChunk(chunk, x, y, z, i,
                         Block::getVertices(Block::Side::eRight, chunk, x, y, z));
        }

        if (y == 0 || chunk.blocks[x][y - 1][z] == Block::Type::eAir) {
          addSideToChunk(chunk, x, y, z, i,
                         Block::getVertices(Block::Side::eBottom, chunk, x, y, z));
        }

        if (y == Chunk::BlocksY - 1 || chunk.blocks[x][y + 1][z] == Block::Type::eAir) {
          addSideToChunk(chunk, x, y, z, i, Block::getVertices(Block::Side::eTop, chunk, x, y, z));
        }

        if (z == 0 || chunk.blocks[x][y][z - 1] == Block::Type::eAir) {
          addSideToChunk(chunk, x, y, z, i, Block::getVertices(Block::Side::eBack, chunk, x, y, z));
        }

        if (z == Chunk::BlocksZ - 1 || chunk.blocks[x][y][z + 1] == Block::Type::eAir) {
          addSideToChunk(chunk, x, y, z, i,
                         Block::getVertices(Block::Side::eFront, chunk, x, y, z));
        }
      }
    }
  }

  return chunk;
}

std::vector<Chunk> ChunkGenerator::generateMany(int const &numX, int const &numZ) {
  std::vector<Chunk> chunks{};

  for (int x = 0; x < numX; x++) {
    for (int z = 0; z < numZ; z++) {
      Chunk chunk = generate();
      chunk.position = glm::vec3{x * Chunk::BlocksX, 0.0f, z * Chunk::BlocksZ};
      chunk.mesh.position = glm::translate(glm::mat4{1.0f}, chunk.position);

      chunks.push_back(chunk);
    }
  }

  return chunks;
}

} // namespace cbl