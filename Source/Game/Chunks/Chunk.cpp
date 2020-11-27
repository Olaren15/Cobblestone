#include "Chunk.hpp"

namespace cbl {

void Chunk::addSideToMesh(
    int const &x, int const &y, int const &z,
    std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>> const &sideData) {

  auto indexOffset = static_cast<uint32_t>(mesh.vertices.size());
  mesh.indices.reserve(sideData.first.size());

  for (uint32_t const &index : sideData.first) {
    mesh.indices.push_back(index + indexOffset);
  }

  mesh.vertices.reserve(sideData.second.size());

  for (gfx::Vertex const &vertex : sideData.second) {
    mesh.vertices.push_back(gfx::Vertex{{vertex.position.x + static_cast<float>(x),
                                         vertex.position.y + static_cast<float>(y),
                                         vertex.position.z + static_cast<float>(z)},
                                        {vertex.uvw}});
  }
}

void Chunk::rebuildMesh() {
  mesh.indices.clear();
  mesh.vertices.clear();

  for (int x = 0; x < Chunk::BlocksX; x++) {
    for (int y = 0; y < Chunk::BlocksY; y++) {
      for (int z = 0; z < Chunk::BlocksZ; z++) {
        Block::Type currentBlock = blocks[x][y][z];

        if (x == 0) {
          if (neighbourXMinus != nullptr) {
            if (neighbourXMinus->blocks[BlocksX - 1][y][z] == Block::Type::eAir) {
              addSideToMesh(x, y, z, Block::getVertices(Block::Side::eLeft, currentBlock));
            }
          } else {
            addSideToMesh(x, y, z, Block::getVertices(Block::Side::eLeft, currentBlock));
          }
        } else if (blocks[x - 1][y][z] == Block::Type::eAir) {
          addSideToMesh(x, y, z, Block::getVertices(Block::Side::eLeft, currentBlock));
        }

        if (x == Chunk::BlocksX - 1) {
          if (neighbourXPlus != nullptr) {
            if (neighbourXPlus->blocks[0][y][z] == Block::Type::eAir) {
              addSideToMesh(x, y, z, Block::getVertices(Block::Side::eRight, currentBlock));
            }
          } else {
            addSideToMesh(x, y, z, Block::getVertices(Block::Side::eRight, currentBlock));
          }
        } else if (blocks[x + 1][y][z] == Block::Type::eAir) {
          addSideToMesh(x, y, z, Block::getVertices(Block::Side::eRight, currentBlock));
        }

        if (y == 0 || blocks[x][y - 1][z] == Block::Type::eAir) {
          addSideToMesh(x, y, z, Block::getVertices(Block::Side::eBottom, currentBlock));
        }

        if (y == Chunk::BlocksY - 1 || blocks[x][y + 1][z] == Block::Type::eAir) {
          addSideToMesh(x, y, z, Block::getVertices(Block::Side::eTop, currentBlock));
        }

        if (z == 0) {
          if (neighbourZMinus != nullptr) {
            if (neighbourZMinus->blocks[x][y][BlocksZ - 1] == Block::Type::eAir) {
              addSideToMesh(x, y, z, Block::getVertices(Block::Side::eBack, currentBlock));
            }
          } else {
            addSideToMesh(x, y, z, Block::getVertices(Block::Side::eBack, currentBlock));
          }
        } else if (blocks[x][y][z - 1] == Block::Type::eAir) {
          addSideToMesh(x, y, z, Block::getVertices(Block::Side::eBack, currentBlock));
        }

        if (z == Chunk::BlocksZ - 1) {
          if (neighbourZPlus != nullptr) {
            if (neighbourZPlus->blocks[x][y][0] == Block::Type::eAir) {
              addSideToMesh(x, y, z, Block::getVertices(Block::Side::eFront, currentBlock));
            }
          } else {
            addSideToMesh(x, y, z, Block::getVertices(Block::Side::eFront, currentBlock));
          }
        } else if (blocks[x][y][z + 1] == Block::Type::eAir) {
          addSideToMesh(x, y, z, Block::getVertices(Block::Side::eFront, currentBlock));
        }
      }
    }
  }
}
} // namespace cbl