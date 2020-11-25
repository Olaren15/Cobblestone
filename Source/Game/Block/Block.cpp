#include "Block.hpp"

namespace cbl {

std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
Block::getFrontVertices(Block::Type const &type) {
  switch (type) {
  case Block::Type::eAir:
    return {{}, {}};
  case Block::Type::eGrass:
    return {{0, 1, 3, 3, 2, 0},
            {{{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
             {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
             {{0.0f, 0.0f, 1.0f}, {0.0, 1.0f, 0.0f}},
             {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}}};
  }

  return {{}, {}};
}

std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
Block::getRightVertices(Block::Type const &type) {
  switch (type) {
  case Block::Type::eAir:
    return {{}, {}};
  case Block::Type::eGrass:
    return {{0, 1, 3, 3, 2, 0},
            {{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
             {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
             {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
             {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}}};
  }

  return {{}, {}};
}

std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
Block::getBackVertices(Block::Type const &type) {
  switch (type) {
  case Block::Type::eAir:
    return {{}, {}};
  case Block::Type::eGrass:
    return {{0, 1, 3, 3, 2, 0},
            {{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
             {{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
             {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
             {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}}};
  }

  return {{}, {}};
}

std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
Block::getLeftVertices(Block::Type const &type) {
  switch (type) {
  case Block::Type::eAir:
    return {{}, {}};
  case Block::Type::eGrass:
    return {{0, 1, 3, 3, 2, 0},
            {{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
             {{0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
             {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
             {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}}};
  }

  return {{}, {}};
}

std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
cbl::Block::getTopVertices(cbl::Block::Type const &type) {
  switch (type) {
  case Block::Type::eAir:
    return {{}, {}};
  case Block::Type::eGrass:
    return {{0, 1, 3, 3, 2, 0},
            {{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
             {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}},
             {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
             {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}}};
  }

  return {{}, {}};
}
std::pair<std::vector<uint32_t>, std::vector<gfx::Vertex>>
Block::getBottomVertices(Block::Type const &type) {
  switch (type) {
  case Block::Type::eAir:
    return {{}, {}};
  case Block::Type::eGrass:
    return {{0, 1, 3, 3, 2, 0},
            {{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 2.0f}},
             {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 2.0f}},
             {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 2.0f}},
             {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 2.0f}}}};
  }

  return {{}, {}};
}

} // namespace cbl
