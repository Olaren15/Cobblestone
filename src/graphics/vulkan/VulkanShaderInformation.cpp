#include "graphics/vulkan/VulkanShaderInformation.hpp"

#include <graphics/Vertex.hpp>

namespace flex {

VkPipelineVertexInputStateCreateInfo VulkanShaderInformation::getVertexInputStateCreateInfo() {
  mVertexInputBindingDescriptions.push_back(Vertex::getVulkanBindingDescription());
  for (VkVertexInputAttributeDescription &attributeDescription :
       Vertex::getVulkanAttributeDescriptions()) {
    mVertexInputAttributeDescriptions.push_back(attributeDescription);
  }

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount =
      static_cast<uint32_t>(mVertexInputBindingDescriptions.size());
  vertexInputStateCreateInfo.pVertexBindingDescriptions = mVertexInputBindingDescriptions.data();
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(mVertexInputAttributeDescriptions.size());
  vertexInputStateCreateInfo.pVertexAttributeDescriptions =
      mVertexInputAttributeDescriptions.data();

  return vertexInputStateCreateInfo;
}

VkPipelineInputAssemblyStateCreateInfo
VulkanShaderInformation::getInputAssemblyStateCreateInfo() const {
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
  inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

  return inputAssemblyStateCreateInfo;
}

VkPipelineViewportStateCreateInfo VulkanShaderInformation::getViewportStateCreateInfo() {
  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = nullptr; // dynamic state
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = nullptr; // dynamic state

  return viewportStateCreateInfo;
}

VkPipelineRasterizationStateCreateInfo
VulkanShaderInformation::getRasterizationStateCreateInfo() const {
  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
  rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
  rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
  rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
  rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
  rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
  rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
  rasterizationStateCreateInfo.lineWidth = 1.0f;

  return rasterizationStateCreateInfo;
}
VkPipelineColorBlendStateCreateInfo VulkanShaderInformation::getColorBlendStateCreateInfo() {

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
  colorBlendAttachmentState.blendEnable = VK_FALSE;
  colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                             VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  mColorBlendAttachmentStates.push_back(colorBlendAttachmentState);

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendStateCreateInfo.attachmentCount =
      static_cast<uint32_t>(mColorBlendAttachmentStates.size());
  colorBlendStateCreateInfo.pAttachments = mColorBlendAttachmentStates.data();

  return colorBlendStateCreateInfo;
}

VkPipelineDynamicStateCreateInfo VulkanShaderInformation::getDynamicStateCreateInfo() {
  mDynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
  dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
  dynamicStateCreateInfo.pDynamicStates = mDynamicStates.data();

  return dynamicStateCreateInfo;
}
VkPipelineMultisampleStateCreateInfo
VulkanShaderInformation::getMultiSampleStateCreateInfo() const {
  VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo{};
  multiSampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multiSampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multiSampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
  multiSampleStateCreateInfo.minSampleShading = 1.0f;
  multiSampleStateCreateInfo.pSampleMask = nullptr;
  multiSampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
  multiSampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

  return multiSampleStateCreateInfo;
}

VkPipelineDepthStencilStateCreateInfo
VulkanShaderInformation::getDepthStencilStateCreateInfo() const {
  VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
  depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
  depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
  depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
  depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

  return depthStencilStateCreateInfo;
}
} // namespace flex