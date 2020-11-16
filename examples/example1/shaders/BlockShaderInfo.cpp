#include "BlockShaderInfo.hpp"

#include <FlexEngine.hpp>

uint32_t BlockShaderInfo::getShaderId() const { return 0; }

std::filesystem::path BlockShaderInfo::getVertSpirVPath() const {
  return std::filesystem::path("shaders/block.vert.spv");
}

std::filesystem::path BlockShaderInfo::getFragSpirVPath() const {
  return std::filesystem::path("shaders/block.frag.spv");
}
