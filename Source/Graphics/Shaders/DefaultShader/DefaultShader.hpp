#pragma once

#include "Graphics/Shaders/BaseShader.hpp"

namespace cbl::gfx {
struct DefaultShader : public BaseShader {
private:
public:
  DefaultShader() = delete;
  DefaultShader(GPU const &gpu, VkRenderPass const &renderPass);

  [[nodiscard]] std::string getName() override;
};
} // namespace cbl::gfx