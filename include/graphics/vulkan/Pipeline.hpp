#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/SwapChain.hpp"

namespace flex {
struct Pipeline {
private:
  [[nodiscard]] VkShaderModule createShaderModule(VkDevice const &device,
                                                  std::filesystem::path const &shaderPath) const;

public:
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;

  Pipeline() = default;
  explicit Pipeline(VkDevice const &device, SwapChain const &swapChain,
                    VkRenderPass const &renderPass);
  void destroy(VkDevice const &device) const;
};
} // namespace flex
