#include "graphics/Shader.hpp"

#include <fstream>

#include "graphics/VulkanHelpers.hpp"

namespace flex {
Shader::Shader(flex::GPU const &gpu, VkRenderPass const &renderPass,
                           VkPipelineLayout const &pipelineLayout, ShaderInformation &shaderInfo) {
  shaderId = shaderInfo.getShaderId();

  mVertShaderModule = createShaderModule(gpu, shaderInfo.getVertSpirVPath());
  mFragShaderModule = createShaderModule(gpu, shaderInfo.getFragSpirVPath());

  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
  shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module = mVertShaderModule;
  shaderStages[0].pName = "main";

  shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module = mFragShaderModule;
  shaderStages[1].pName = "main";

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo =
      shaderInfo.getVertexInputStateCreateInfo();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
      shaderInfo.getInputAssemblyStateCreateInfo();

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
      shaderInfo.getViewportStateCreateInfo();

  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
      shaderInfo.getRasterizationStateCreateInfo();

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo =
      shaderInfo.getColorBlendStateCreateInfo();

  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = shaderInfo.getDynamicStateCreateInfo();

  VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo =
      shaderInfo.getMultiSampleStateCreateInfo();

  VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo =
      shaderInfo.getDepthStencilStateCreateInfo();

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
      vkCreateGraphicsPipelines(gpu.device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline));
}

VkShaderModule Shader::createShaderModule(GPU const &gpu,
                                                std::filesystem::path const &path) {
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
      vkCreateShaderModule(gpu.device, &shaderModuleCreateInfo, nullptr, &shaderModule));

  return shaderModule;
}

void Shader::destroy(flex::GPU const &gpu) {
  vkDestroyPipeline(gpu.device, pipeline, nullptr);
  vkDestroyShaderModule(gpu.device, mFragShaderModule, nullptr);
  vkDestroyShaderModule(gpu.device, mVertShaderModule, nullptr);
}

} // namespace flex
