#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/VulkanGPU.hpp"
#include "graphics/vulkan/VulkanShaderInformation.hpp"

namespace flex {
struct VulkanShader {
private:
  VkShaderModule mVertShaderModule;
  VkShaderModule mFragShaderModule;

  [[nodiscard]] static VkShaderModule createShaderModule(VulkanGPU const &gpu,
                                                         std::filesystem::path const &path);

public:
  VkPipeline pipeline{};
  uint32_t shaderId;

  VulkanShader() = delete;
  VulkanShader(VulkanGPU const &gpu, VkRenderPass const &renderPass,
               VkPipelineLayout const &pipelineLayout, VulkanShaderInformation &shaderInfo);

  void destroy(VulkanGPU const &gpu);
};
} // namespace flex