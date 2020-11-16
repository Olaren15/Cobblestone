#pragma once

#include <filesystem>
#include <vector>

#include <vulkan/vulkan.h>

namespace flex {
struct VulkanShaderInformation {
protected:
  // pipeline layout
  std::vector<VkDescriptorSetLayout> mDescriptorSetsLayouts;
  std::vector<VkPushConstantRange> mPushConstantRanges;

  // vertex input
  std::vector<VkVertexInputBindingDescription> mVertexInputBindingDescriptions;
  std::vector<VkVertexInputAttributeDescription> mVertexInputAttributeDescriptions;

  // viewport state
  std::vector<VkViewport> mViewports;
  std::vector<VkRect2D> mScissors;

  // color blend
  std::vector<VkPipelineColorBlendAttachmentState> mColorBlendAttachmentStates;

  // dynamic state
  std::vector<VkDynamicState> mDynamicStates;

public:
  [[nodiscard]] virtual uint32_t getShaderId() const = 0;

  [[nodiscard]] virtual std::filesystem::path getVertSpirVPath() const = 0;
  [[nodiscard]] virtual std::filesystem::path getFragSpirVPath() const = 0;

  [[nodiscard]] virtual VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo();

  [[nodiscard]] virtual VkPipelineInputAssemblyStateCreateInfo
  getInputAssemblyStateCreateInfo() const;

  [[nodiscard]] virtual VkPipelineViewportStateCreateInfo getViewportStateCreateInfo();

  [[nodiscard]] virtual VkPipelineRasterizationStateCreateInfo
  getRasterizationStateCreateInfo() const;

  [[nodiscard]] virtual VkPipelineColorBlendStateCreateInfo getColorBlendStateCreateInfo();

  [[nodiscard]] virtual VkPipelineDynamicStateCreateInfo getDynamicStateCreateInfo();

  [[nodiscard]] virtual VkPipelineMultisampleStateCreateInfo getMultiSampleStateCreateInfo() const;

  [[nodiscard]] virtual VkPipelineDepthStencilStateCreateInfo
  getDepthStencilStateCreateInfo() const;
};
} // namespace flex