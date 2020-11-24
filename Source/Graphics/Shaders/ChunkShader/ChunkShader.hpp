#pragma once

#include "Graphics/Shaders/BaseShader.hpp"

namespace cbl::gfx {
struct ChunkShader : public BaseShader {
private:
public:
  ChunkShader() = delete;
  ChunkShader(GPU const &gpu, VkRenderPass const &renderPass);

  [[nodiscard]] std::string getName() override;
};
} // namespace cbl::gfx