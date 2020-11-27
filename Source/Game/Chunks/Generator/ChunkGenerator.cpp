#include "ChunkGenerator.hpp"

#include <ctime>

#include <glm/gtc/matrix_transform.hpp>

#include "External/PerlinNoise/PerlinNoise.hpp"

namespace cbl {

Chunk ChunkGenerator::generate(int const &posX, int const &posZ) {
  Chunk chunk{};
  chunk.position = glm::vec3{posX * Chunk::BlocksX, 0.0f, posZ * Chunk::BlocksZ};
  chunk.mesh.position = glm::translate(glm::mat4{1.0f}, chunk.position);

  siv::PerlinNoise perlin(std::time(nullptr));
  double frequency = 50.0f;

  for (int x = 0; x < Chunk::BlocksX; x++) {
    for (int y = 0; y < Chunk::BlocksY; y++) {
      for (int z = 0; z < Chunk::BlocksZ; z++) {
        double noiseValue = perlin.accumulatedOctaveNoise2D_0_1(
            static_cast<double>(x + static_cast<int>(chunk.position.x)) / frequency,
            static_cast<double>(z + static_cast<int>(chunk.position.z)) / frequency, 3);
        int adjustedNoiseValue = floor(noiseValue * Chunk::BlocksY);

        if (y > adjustedNoiseValue) {
          chunk.blocks[x][y][z] = Block::Type::eAir;
        } else if (y == adjustedNoiseValue) {
          chunk.blocks[x][y][z] = Block::Type::eGrass;
        } else {
          chunk.blocks[x][y][z] = Block::Type::eDirt;
        }
      }
    }
  }

  return chunk;
}

std::map<std::pair<int, int>, Chunk> ChunkGenerator::generateMany(int const &numX,
                                                                  int const &numZ) {
  std::map<std::pair<int, int>, Chunk> chunks{};

  for (int x = 0; x < numX; x++) {
    for (int z = 0; z < numZ; z++) {
      chunks[std::make_pair(x, z)] = generate(x, z);
    }
  }

  for (int x = 0; x < numX; x++) {
    for (int z = 0; z < numZ; z++) {
      Chunk &currentChunk = chunks[std::make_pair(x, z)];

      if (x != 0) {
        currentChunk.neighbourXMinus = &chunks[std::make_pair(x - 1, z)];
      }

      if (x != numX - 1) {
        currentChunk.neighbourXPlus = &chunks[std::make_pair(x + 1, z)];
      }

      if (z != 0) {
        currentChunk.neighbourZMinus = &chunks[std::make_pair(x, z - 1)];
      }

      if (z != numZ - 1) {
        currentChunk.neighbourZPlus = &chunks[std::make_pair(x, z + 1)];
      }

      currentChunk.rebuildMesh();
    }
  }

  return chunks;
}

} // namespace cbl