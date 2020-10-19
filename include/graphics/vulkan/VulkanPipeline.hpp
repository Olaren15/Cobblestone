#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

namespace flex {
struct VulkanPipeline {
private:
  VkShaderModule mVertShaderModule;
  VkShaderModule mFragShaderModule;

  [[nodiscard]] static VkShaderModule createShaderModule(VkDevice const &device,
                                                  std::filesystem::path const &shaderPath) ;

public:
  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;

  VulkanPipeline() = default;
  VulkanPipeline(VulkanPipeline const &pipeline) = delete;

  void createPipeline(VkDevice const &device, VkRenderPass const &renderPass);
  void destroy(VkDevice const &device) const;
};
} // namespace flex
