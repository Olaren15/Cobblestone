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
  uint32_t shaderId;

  Shader() = delete;
  Shader(GPU const &gpu, VkRenderPass const &renderPass,
               VkPipelineLayout const &pipelineLayout, ShaderInformation &shaderInfo);

  void destroy(GPU const &gpu);
};
} // namespace flex