#include "graphics/vulkan/VulkanMemoryManager.hpp"

#include <thread>

#include "graphics/vulkan/VulkanCommandBufferRecorder.hpp"
#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {

void VulkanMemoryManager::allocateBuffer(VkBufferCreateInfo const &bufferInfo,
                                         VmaAllocationCreateInfo const &allocInfo,
                                         VulkanBuffer &buffer) {

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferInfo, &allocInfo, &buffer.buffer,
                                   &buffer.allocation, nullptr));
  buffer.size = bufferInfo.size;
  buffer.memoryManager = this;
  buffer.isValid = true;
}

VulkanBuffer VulkanMemoryManager::createStagingBuffer(VkDeviceSize const &bufferSize) {

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = bufferSize;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferCreateInfo.queueFamilyIndexCount = 1;
  bufferCreateInfo.pQueueFamilyIndices = &mGPU.queueFamilyIndices.transfer.value();
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VulkanBuffer stagingBuffer{};
  allocateBuffer(bufferCreateInfo, allocationCreateInfo, stagingBuffer);
  return stagingBuffer;
}

void VulkanMemoryManager::destroyBufferOnFenceTrigger(VulkanBuffer buffer, VkFence fence) const {
  vkWaitForFences(mGPU.device, 1, &fence, VK_TRUE, UINT64_MAX);
  vkDestroyFence(mGPU.device, fence, nullptr);
  destroyBuffer(buffer);
}

void VulkanMemoryManager::initialise(VulkanGPU const &gpu) {
  mGPU = gpu;

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

void VulkanMemoryManager::destroy() const {
  vkDestroyCommandPool(mGPU.device, mTransferCommandPool, nullptr);
  vmaDestroyAllocator(mAllocator);
}

void VulkanMemoryManager::destroyBuffer(VulkanBuffer &buffer) const {
  vmaDestroyBuffer(mAllocator, buffer.buffer, buffer.allocation);
  buffer.isValid = false;
}

void VulkanMemoryManager::generateMeshBuffer(Mesh &mesh) {

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

void VulkanMemoryManager::updateMeshBuffer(Mesh &mesh) {
  VulkanBuffer stagingBuffer = createStagingBuffer(mesh.getRequiredBufferSize());

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

  VulkanCommandBufferRecorder recorder{mTransferCommandBuffer};
  recorder.beginOneTime()
      .copyBuffer(stagingBuffer, mesh.buffer)
      .addStagingBufferMemoryBarrier(mesh.buffer, mGPU.queueFamilyIndices)
      .end()
      .submitWithFence(mGPU.transferQueue, bufferCopiedFence);

  // wait until the transfer is complete before deleting the staging buffer
  std::thread thread{&VulkanMemoryManager::destroyBufferOnFenceTrigger, this, stagingBuffer,
                     bufferCopiedFence};
  thread.detach();
}

VulkanImage VulkanMemoryManager::createImage(VkExtent2D const &extent, VkFormat const &format,
                                             VkImageTiling const &tiling,
                                             VkImageUsageFlags const &usage,
                                             VkImageAspectFlags const &imageAspect) {
  VulkanImage image{};
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

void VulkanMemoryManager::destroyImage(VulkanImage &image) {
  vkDestroyImageView(mGPU.device, image.imageView, nullptr);
  vmaDestroyImage(mAllocator, image.image, image.allocation);
}

void VulkanMemoryManager::createImageView(VulkanImage &image,
                                          VkImageAspectFlags const &imageAspect) const {
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