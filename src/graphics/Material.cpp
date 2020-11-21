#include "graphics/Material.hpp"

#include "graphics/VulkanHelpers.hpp"

namespace flex {

Material::Material(GPU const &gpu, MemoryManager &memoryManager, Shader const &shader)
    : mMemoryManager{memoryManager} {
  texture = mMemoryManager.createTexture("assets/grass_block_side.png");

  VkDescriptorSetAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocateInfo.descriptorPool = shader.descriptorPool;
  allocateInfo.descriptorSetCount = 1;
  allocateInfo.pSetLayouts = &shader.descriptorSetLayout;
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
}

Material::~Material() { mMemoryManager.destroyTexture(texture); }
} // namespace flex
