#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

#include "GPU.hpp"
#include "ShaderInformation.hpp"

namespace flex {
struct Shader {
private:
  VkShaderModule mVertShaderModule;
  VkShaderModule mFragShaderModule;

  [[nodiscard]] static VkShaderModule createShaderModule(GPU const &gpu,
                                                         std::filesystem::path const &path);

public:
  VkPipeline pipeline{};
  VkPipelineLayout pipelineLayout{};
  uint32_t shaderId;

  Shader() = delete;
  Shader(GPU const &gpu, VkRenderPass const &renderPass, ShaderInformation &shaderInfo);

  void destroy(GPU const &gpu);
};
} // namespace flex