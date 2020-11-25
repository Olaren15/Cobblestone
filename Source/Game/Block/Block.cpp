#include "Block.hpp"

#include "Game/Chunks/Chunk.hpp"

namespace cbl {

std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
Block::getVertices(Block::Side const &side, Chunk const &chunk, int const &x, int const &y,
                   int const &z) {
  std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>> sideData{{}, {}};
  Type currentBlock = chunk.blocks[x][y][z];

  switch (side) {
  case Side::eFront:
    switch (currentBlock) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                   {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
                   {{0.0f, 0.0f, 1.0f}, {0.0, 1.0f, 0.0f}},
                   {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}}};
      break;
    }
    break;
  case Side::eRight:
    switch (currentBlock) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                   {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                   {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                   {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}}};
      break;
    }
    break;
  case Side::eBack:
    switch (currentBlock) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                   {{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                   {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                   {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}}};
      break;
    }
    break;
  case Side::eLeft:
    switch (currentBlock) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                   {{0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
                   {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                   {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}}};
      break;
    }
    break;
  case Side::eTop:
    switch (currentBlock) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                   {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}},
                   {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
                   {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}}};
      break;
    }
    break;
  case Side::eBottom:
    switch (currentBlock) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 2.0f}},
                   {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 2.0f}},
                   {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 2.0f}},
                   {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 2.0f}}}};
      break;
    }
    break;
  }

  return sideData;
}

} // namespace cbl
