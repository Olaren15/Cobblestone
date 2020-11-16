#pragma once

#include <FlexEngine.hpp>

struct BlockShaderInfo : public flex::ShaderInformation {
public:
  [[nodiscard]] uint32_t getShaderId() const override;

  [[nodiscard]] std::filesystem::path getVertSpirVPath() const override;
  [[nodiscard]] std::filesystem::path getFragSpirVPath() const override;
};
