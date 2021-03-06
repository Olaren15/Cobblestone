#include "BaseShader.hpp"

#include <array>
#include <fstream>

#include "Graphics/Utils/VulkanHelpers.hpp"

namespace cbl::gfx {
BaseShader::BaseShader(GPU const &gpu, VkRenderPass const &renderPass) : mGPU{gpu} {}

VkShaderModule BaseShader::createShaderModule(std::filesystem::path const &path) const {
  std::ifstream shaderFile{path.string(), std::ios::ate | std::ios::binary};

  if (!shaderFile.is_open()) {
    throw std::runtime_error{"Failed to open file " + path.string()};
  }

  std::streamsize const fileSize = shaderFile.tellg();
  std::vector<char> buffer(static_cast<size_t>(fileSize));

  shaderFile.seekg(0);
  shaderFile.read(buffer.data(), fileSize);
  shaderFile.close();

  VkShaderModuleCreateInfo shaderModuleCreateInfo{};
  shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleCreateInfo.codeSize = static_cast<uint32_t>(buffer.size());
  shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

  VkShaderModule shaderModule{};
  validateVkResult(
      vkCreateShaderModule(mGPU.device, &shaderModuleCreateInfo, nullptr, &shaderModule));

  return shaderModule;
}

void BaseShader::createDefaultPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  validateVkResult(
      vkCreatePipelineLayout(mGPU.device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
}

void BaseShader::createDefaultPipeline(VkRenderPass const &renderPass) {
  std::filesystem::path shaderPath{"Shaders/" + getName() + "/" + getName()};

  VkShaderModule vertShaderModule = createShaderModule({shaderPath.string() + ".vert.spv"});
  VkShaderModule fragShaderModule = createShaderModule({shaderPath.string() + ".frag.spv"});

  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
  shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module = vertShaderModule;
  shaderStages[0].pName = "main";

  shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module = fragShaderModule;
  shaderStages[1].pName = "main";

  VkVertexInputBindingDescription bindingDescription = Vertex::getVulkanBindingDescription();
  std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions =
      Vertex::getVulkanAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
  inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = nullptr; // dynamic state
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = nullptr; // dynamic state

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

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

  std::array<VkDynamicState, 2> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
  dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

  VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo{};
  multiSampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multiSampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multiSampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
  multiSampleStateCreateInfo.minSampleShading = 1.0f;
  multiSampleStateCreateInfo.pSampleMask = nullptr;
  multiSampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
  multiSampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

  VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
  depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
  depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
  depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
  depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

  VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipelineCreateInfo.pStages = shaderStages.data();
  pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
  pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
  pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
  pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
  pipelineCreateInfo.pMultisampleState = &multiSampleStateCreateInfo;
  pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
  pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
  pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
  pipelineCreateInfo.layout = pipelineLayout;
  pipelineCreateInfo.renderPass = renderPass;
  pipelineCreateInfo.subpass = 0;
  pipelineCreateInfo.basePipelineIndex = -1;

  validateVkResult(
      vkCreateGraphicsPipelines(mGPU.device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline));

  vkDestroyShaderModule(mGPU.device, vertShaderModule, nullptr);
  vkDestroyShaderModule(mGPU.device, fragShaderModule, nullptr);
}
BaseShader::~BaseShader() {
  vkDestroyDescriptorPool(mGPU.device, descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(mGPU.device, descriptorSetLayout, nullptr);
  vkDestroyPipeline(mGPU.device, pipeline, nullptr);
  vkDestroyPipelineLayout(mGPU.device, pipelineLayout, nullptr);
}

} // namespace cbl::gfx
