#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/vulkanSwapchain.hpp"

namespace flex {
struct Pipeline {
private:
  [[nodiscard]] VkShaderModule createShaderModule(VkDevice const &device,
                                                  std::filesystem::path const &shaderPath) const;

public:
  static constexpr std::array<VkDynamicState, 2> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT,
                                                               VK_DYNAMIC_STATE_SCISSOR};

  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
  VkDynamicState dynamicState;
  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;

  Pipeline() = default;
  Pipeline(Pipeline const &pipeline) = delete;

  void createPipeline(VkDevice const &device, VkRenderPass const &renderPass);
  void destroy(VkDevice const &device) const;
};
} // namespace flex
