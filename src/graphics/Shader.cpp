#include "graphics/Shader.hpp"

#include <array>
#include <fstream>

#include "graphics/VulkanHelpers.hpp"

namespace flex {
Shader::Shader(flex::GPU const &gpu, VkRenderPass const &renderPass,
                       ShaderInformation &shaderInfo, MemoryManager &memoryManager)
    : mMemoryManager{memoryManager} {
  shaderId = shaderInfo.getShaderId();

  texture = mMemoryManager.createTexture("assets/grass_block_side.png");

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(glm::mat4) * 2;

  VkDescriptorSetLayoutBinding samplerBinding{};
  samplerBinding.binding = 1;
  samplerBinding.descriptorCount = 1;
  samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBinding.pImmutableSamplers = nullptr;
  samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
  descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutCreateInfo.bindingCount = 1;
  descriptorSetLayoutCreateInfo.pBindings = &samplerBinding;

  VkDescriptorSetLayout descriptorSetLayout;
  validateVkResult(vkCreateDescriptorSetLayout(gpu.device, &descriptorSetLayoutCreateInfo, nullptr,
                                               &descriptorSetLayout));

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
  pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
  validateVkResult(
      vkCreatePipelineLayout(gpu.device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

  VkDescriptorPoolSize poolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1};

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
  descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCreateInfo.poolSizeCount = 1;
  descriptorPoolCreateInfo.pPoolSizes = &poolSize;
  descriptorPoolCreateInfo.maxSets = 1;
  validateVkResult(
      vkCreateDescriptorPool(gpu.device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

  VkDescriptorSetAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocateInfo.descriptorPool = descriptorPool;
  allocateInfo.descriptorSetCount = 1;
  allocateInfo.pSetLayouts = &descriptorSetLayout;
  validateVkResult(vkAllocateDescriptorSets(gpu.device, &allocateInfo, &descriptorSet));

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageView = texture.image.imageView;
  imageInfo.sampler = texture.sampler;
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.dstSet = descriptorSet;
  writeDescriptorSet.dstBinding = 1;
  writeDescriptorSet.dstArrayElement = 0;
  writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writeDescriptorSet.descriptorCount = 1;
  writeDescriptorSet.pImageInfo = &imageInfo;
  vkUpdateDescriptorSets(gpu.device, 1, &writeDescriptorSet, 0, nullptr);

  vkDestroyDescriptorSetLayout(gpu.device, descriptorSetLayout, nullptr);

  VkShaderModule vertShaderModule = createShaderModule(gpu, shaderInfo.getVertSpirVPath());
  VkShaderModule fragShaderModule = createShaderModule(gpu, shaderInfo.getFragSpirVPath());

  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
  shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module = vertShaderModule;
  shaderStages[0].pName = "main";

  shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module = fragShaderModule;
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

  vkDestroyShaderModule(gpu.device, vertShaderModule, nullptr);
  vkDestroyShaderModule(gpu.device, fragShaderModule, nullptr);
}

VkShaderModule Shader::createShaderModule(GPU const &gpu, std::filesystem::path const &path) {
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
  mMemoryManager.destroyTexture(texture);
  vkDestroyDescriptorPool(gpu.device, descriptorPool, nullptr);
  vkDestroyPipeline(gpu.device, pipeline, nullptr);
  vkDestroyPipelineLayout(gpu.device, pipelineLayout, nullptr);
}

} // namespace flex
