#include "DefaultShader.hpp"

#include "Graphics/Utils/VulkanHelpers.hpp"

namespace cbl::gfx {

DefaultShader::DefaultShader(GPU const &gpu, VkRenderPass const &renderPass)
    : BaseShader(gpu, renderPass) {

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

  validateVkResult(vkCreateDescriptorSetLayout(mGPU.device, &descriptorSetLayoutCreateInfo, nullptr,
                                               &descriptorSetLayout));

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(glm::mat4) * 2;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
  pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
  validateVkResult(
      vkCreatePipelineLayout(mGPU.device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

  VkDescriptorPoolSize poolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1};

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
  descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCreateInfo.poolSizeCount = 1;
  descriptorPoolCreateInfo.pPoolSizes = &poolSize;
  descriptorPoolCreateInfo.maxSets = 1;
  validateVkResult(
      vkCreateDescriptorPool(mGPU.device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

  createDefaultPipeline(renderPass);
}

std::string DefaultShader::getName() { return "Default"; }

} // namespace cbl::gfx
