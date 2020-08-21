#pragma once

#include <filesystem>

#include <vulkan/vulkan.hpp>

#include "graphics/vulkan/SwapChain.hpp"

namespace flex {
struct Pipeline {
private:
  vk::ShaderModule vertShaderModule;
  vk::ShaderModule fragShaderModule;
  vk::PipelineLayout pipelineLayout;
  vk::RenderPass renderPass;
  vk::Pipeline pipeline;

  [[nodiscard]] vk::ShaderModule createShaderModule(vk::Device const &device,
                                                    std::filesystem::path const &shaderPath) const;
  [[nodiscard]] vk::PipelineViewportStateCreateInfo
  buildViewPortStateCreateInfo(vk::Extent2D const &swapChainExtent);
  [[nodiscard]] vk::PipelineRasterizationStateCreateInfo buildRasterizationStateCreateInfo();
  [[nodiscard]] vk::PipelineColorBlendStateCreateInfo buildColorBlendStateCreateInfo();
  [[nodiscard]] vk::RenderPass createRenderPass(vk::Device const &device,
                                                vk::Format const &swapChainFormat);

public:
  Pipeline() = default;
  explicit Pipeline(vk::Device const &device, SwapChain const &swapChain);
  void destroy(vk::Device const &device) const;
};
} // namespace flex
