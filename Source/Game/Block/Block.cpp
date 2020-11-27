#include "Block.hpp"

#include "Game/Chunks/Chunk.hpp"

namespace cbl {

std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
Block::getVertices(Block::Side const &side, Type const &type) {
  std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>> sideData{{}, {}};

  switch (side) {
  case Side::eFront:
    switch (type) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                   {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
                   {{0.0f, 0.0f, 1.0f}, {0.0, 1.0f, 0.0f}},
                   {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}}};
      break;
    case Type::eDirt:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 2.0f}},
                   {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 2.0f}},
                   {{0.0f, 0.0f, 1.0f}, {0.0, 1.0f, 2.0f}},
                   {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 2.0f}}}};
      break;
    }
    break;
  case Side::eRight:
    switch (type) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                   {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                   {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
                   {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}}};
      break;
    case Type::eDirt:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 2.0f}},
                   {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 2.0f}},
                   {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 2.0f}},
                   {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 2.0f}}}};
      break;
    }
    break;
  case Side::eBack:
    switch (type) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                   {{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                   {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                   {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}}};
      break;
    case Type::eDirt:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 2.0f}},
                   {{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 2.0f}},
                   {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 2.0f}},
                   {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 2.0f}}}};
      break;
    }
    break;
  case Side::eLeft:
    switch (type) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                   {{0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
                   {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                   {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}}};
      break;
    case Type::eDirt:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 2.0f}},
                   {{0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 2.0f}},
                   {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 2.0f}},
                   {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 2.0f}}}};
      break;
    }
    break;
  case Side::eTop:
    switch (type) {
    case Type::eAir:
      break;
    case Type::eGrass:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                   {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}},
                   {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
                   {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}}};
      break;
    case Type::eDirt:
      sideData = {{0, 1, 3, 3, 2, 0},
                  {{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 2.0f}},
                   {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 2.0f}},
                   {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 2.0f}},
                   {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 2.0f}}}};
      break;
    }
    break;
  case Side::eBottom:
    switch (type) {
    case Type::eAir:
      break;
    case Type::eGrass:
    case Type::eDirt:
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
