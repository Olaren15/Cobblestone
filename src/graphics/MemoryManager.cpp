#include "graphics/MemoryManager.hpp"

#include <thread>

#include "external/stb_image.h"

#include "graphics/CommandBufferRecorder.hpp"
#include "graphics/VulkanHelpers.hpp"

namespace flex {

MemoryManager::MemoryManager(GPU const &gpu) : mGPU{gpu} {

  VmaAllocatorCreateInfo allocatorCreateInfo{};
  allocatorCreateInfo.instance = mGPU.instance;
  allocatorCreateInfo.physicalDevice = mGPU.physicalDevice;
  allocatorCreateInfo.device = mGPU.device;

  validateVkResult(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));

  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags =
      VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = mGPU.queueFamilyIndices.transfer.value();

  validateVkResult(
      vkCreateCommandPool(mGPU.device, &commandPoolCreateInfo, nullptr, &mTransferCommandPool));

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandPool = mTransferCommandPool;
  commandBufferAllocateInfo.commandBufferCount = 1;

  validateVkResult(
      vkAllocateCommandBuffers(mGPU.device, &commandBufferAllocateInfo, &mTransferCommandBuffer));
}

MemoryManager::~MemoryManager() {
  mGPU.waitIdle();
  vkDestroyCommandPool(mGPU.device, mTransferCommandPool, nullptr);
  vmaDestroyAllocator(mAllocator);
}

void MemoryManager::allocateBuffer(VkBufferCreateInfo const &bufferInfo,
                                   VmaAllocationCreateInfo const &allocInfo, Buffer &buffer) {

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferInfo, &allocInfo, &buffer.buffer,
                                   &buffer.allocation, nullptr));
  buffer.size = bufferInfo.size;
  buffer.memoryManager = this;
  buffer.isValid = true;
}

Buffer MemoryManager::createStagingBuffer(VkDeviceSize const &bufferSize) {

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = bufferSize;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferCreateInfo.queueFamilyIndexCount = 1;
  bufferCreateInfo.pQueueFamilyIndices = &mGPU.queueFamilyIndices.transfer.value();
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  Buffer stagingBuffer{};
  allocateBuffer(bufferCreateInfo, allocationCreateInfo, stagingBuffer);
  return stagingBuffer;
}

void MemoryManager::destroyBufferOnFenceTrigger(Buffer buffer, VkFence fence) const {
  vkWaitForFences(mGPU.device, 1, &fence, VK_TRUE, UINT64_MAX);
  vkDestroyFence(mGPU.device, fence, nullptr);
  destroyBuffer(buffer);
}

void MemoryManager::destroyBuffer(Buffer &buffer) const {
  vmaDestroyBuffer(mAllocator, buffer.buffer, buffer.allocation);
  buffer.isValid = false;
}

void MemoryManager::generateMeshBuffer(Mesh &mesh) {

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = mesh.getRequiredBufferSize();
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferCreateInfo.queueFamilyIndexCount = 1;
  bufferCreateInfo.pQueueFamilyIndices = &mGPU.queueFamilyIndices.transfer.value();
  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  allocateBuffer(bufferCreateInfo, allocationCreateInfo, mesh.buffer);

  updateMeshBuffer(mesh);
}

void MemoryManager::updateMeshBuffer(Mesh &mesh) {
  Buffer stagingBuffer = createStagingBuffer(mesh.getRequiredBufferSize());

  void *mappedMemory;
  vmaMapMemory(mAllocator, stagingBuffer.allocation, &mappedMemory);
  memcpy(mappedMemory, mesh.indices.data(), mesh.getIndicesSize());
  memcpy(static_cast<char *>(mappedMemory) + mesh.getIndicesSize(), mesh.vertices.data(),
         mesh.getVerticesSize());
  vmaUnmapMemory(mAllocator, stagingBuffer.allocation);

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  VkFence bufferCopiedFence{};
  validateVkResult(vkCreateFence(mGPU.device, &fenceCreateInfo, nullptr, &bufferCopiedFence));

  CommandBufferRecorder recorder{mTransferCommandBuffer};
  recorder.beginOneTime()
      .copyBuffer(stagingBuffer, mesh.buffer)
      .addMeshBufferMemoryBarrier(mesh.buffer, mGPU.queueFamilyIndices)
      .end()
      .submitWithFence(mGPU.transferQueue, bufferCopiedFence);

  // wait until the transfer is complete before deleting the staging buffer
  std::thread thread{&MemoryManager::destroyBufferOnFenceTrigger, this, stagingBuffer,
                     bufferCopiedFence};
  thread.detach();
}

Texture MemoryManager::createTexture(std::filesystem::path const &texturePath) {
  int width, height, channels;
  stbi_uc *pixels =
      stbi_load(texturePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if (pixels == nullptr) {
    throw std::runtime_error("Failed to load image");
  }

  VkDeviceSize imageSize = width * height * channels;

  Texture texture{};
  Buffer stagingBuffer = createStagingBuffer(imageSize);

  void *data;
  vmaMapMemory(mAllocator, stagingBuffer.allocation, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vmaUnmapMemory(mAllocator, stagingBuffer.allocation);

  stbi_image_free(pixels);

  texture.image = createImage({static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
                              VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                              VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                              VK_IMAGE_ASPECT_COLOR_BIT);

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

  VkFence transferFinishedFence{};
  validateVkResult(vkCreateFence(mGPU.device, &fenceCreateInfo, nullptr, &transferFinishedFence));

  CommandBufferRecorder recorder{mTransferCommandBuffer};
  recorder.beginOneTime()
      .transitionImageLayout(texture.image, VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mGPU.queueFamilyIndices)
      .copyBufferToImage(stagingBuffer, texture.image)
      .transitionImageLayout(texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mGPU.queueFamilyIndices)
      .end()
      .submitWithFence(mGPU.transferQueue, transferFinishedFence);

  destroyBufferOnFenceTrigger(stagingBuffer, transferFinishedFence);

  VkSamplerCreateInfo samplerCreateInfo{};
  samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
  samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
  samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.anisotropyEnable = VK_TRUE;
  samplerCreateInfo.maxAnisotropy = 16;
  samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
  samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
  samplerCreateInfo.compareEnable = VK_FALSE;
  samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerCreateInfo.mipLodBias = 0.0f;
  samplerCreateInfo.minLod = 0.0f;
  samplerCreateInfo.maxLod = 0.0f;

  validateVkResult(vkCreateSampler(mGPU.device, &samplerCreateInfo, nullptr, &texture.sampler));

  return texture;
}

void MemoryManager::destroyTexture(Texture &texture) {
  vkDestroySampler(mGPU.device, texture.sampler, nullptr);
  destroyImage(texture.image);
}

Image MemoryManager::createImage(VkExtent2D const &extent, VkFormat const &format,
                                 VkImageTiling const &tiling, VkImageUsageFlags const &usage,
                                 VkImageAspectFlags const &imageAspect) {
  Image image{};
  image.format = format;
  image.extent = extent;

  // image memory
  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.extent.width = extent.width;
  imageCreateInfo.extent.height = extent.height;
  imageCreateInfo.extent.depth = 1;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.format = format;
  imageCreateInfo.tiling = tiling;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageCreateInfo.usage = usage;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  validateVkResult(vmaCreateImage(mAllocator, &imageCreateInfo, &allocationCreateInfo, &image.image,
                                  &image.allocation, nullptr));

  createImageView(image, imageAspect);

  return image;
}

void MemoryManager::destroyImage(Image &image) {
  vkDestroyImageView(mGPU.device, image.imageView, nullptr);
  vmaDestroyImage(mAllocator, image.image, image.allocation);
}

void MemoryManager::createImageView(Image &image, VkImageAspectFlags const &imageAspect) const {
  VkImageSubresourceRange subresourceRange{};
  subresourceRange.aspectMask = imageAspect;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  VkComponentMapping componentMapping{};
  componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.image = image.image;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = image.format;
  imageViewCreateInfo.components = componentMapping;
  imageViewCreateInfo.subresourceRange = subresourceRange;

  validateVkResult(vkCreateImageView(mGPU.device, &imageViewCreateInfo, nullptr, &image.imageView));
}

} // namespace flex