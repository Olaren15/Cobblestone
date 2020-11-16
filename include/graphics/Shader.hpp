#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

#include "ShaderInformation.hpp"
#include "graphics/vulkan/VulkanGPU.hpp"

namespace flex {
struct Shader {
private:
  VkShaderModule mVertShaderModule;
  VkShaderModule mFragShaderModule;

  [[nodiscard]] static VkShaderModule createShaderModule(VulkanGPU const &gpu,
                                                         std::filesystem::path const &path);

public:
  VkPipeline pipeline{};
  uint32_t shaderId;

  Shader() = delete;
  Shader(VulkanGPU const &gpu, VkRenderPass const &renderPass,
               VkPipelineLayout const &pipelineLayout, ShaderInformation &shaderInfo);

  void destroy(VulkanGPU const &gpu);
};
} // namespace flex