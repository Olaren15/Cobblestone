#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/VulkanGPU.hpp"

namespace flex {
struct VulkanPipeline {
private:
  VkShaderModule mVertShaderModule;
  VkShaderModule mFragShaderModule;

  [[nodiscard]] static VkShaderModule createShaderModule(VulkanGPU const &gpu,
                                                         std::filesystem::path const &shaderPath);

public:
  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;

  VulkanPipeline() = default;
  VulkanPipeline(VulkanPipeline const &pipeline) = delete;

  void createPipeline(VulkanGPU const &gpu, VkRenderPass const &renderPass);
  void destroy(VulkanGPU const &gpu) const;
};
} // namespace flex
