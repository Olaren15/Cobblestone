#include "ChunkMaterial.hpp"

#include "Graphics/Utils/VulkanHelpers.hpp"

namespace cbl::gfx {

ChunkMaterial::ChunkMaterial(GPU const &gpu, mem::MemoryManager &memoryManager,
                             BaseShader const *shader)
    : BaseMaterial(gpu, memoryManager, shader) {
  texture = mMemoryManager.createTexture(
      {"Assets/grass_block_side.png", "Assets/grass_block_top.png", "Assets/dirt.png"}, true);

  VkDescriptorSet textureDescriptor{};

  VkDescriptorSetAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocateInfo.descriptorPool = shader->descriptorPool;
  allocateInfo.descriptorSetCount = 1;
  allocateInfo.pSetLayouts = &shader->descriptorSetLayout;
  validateVkResult(vkAllocateDescriptorSets(gpu.device, &allocateInfo, &textureDescriptor));

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageView = texture.image.imageView;
  imageInfo.sampler = texture.sampler;
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.dstSet = textureDescriptor;
  writeDescriptorSet.dstArrayElement = 0;
  writeDescriptorSet.dstBinding = 1;
  writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writeDescriptorSet.descriptorCount = 1;
  writeDescriptorSet.pImageInfo = &imageInfo;
  vkUpdateDescriptorSets(gpu.device, 1, &writeDescriptorSet, 0, nullptr);

  descriptorSets.push_back(textureDescriptor);
}

ChunkMaterial::~ChunkMaterial() { mMemoryManager.destroyTexture(texture); }
} // namespace cbl::gfx