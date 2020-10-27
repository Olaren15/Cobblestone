#include "graphics/vulkan/VulkanMemoryManager.hpp"

#include <cstring>

#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {

VkBufferCreateInfo
VulkanMemoryManager::buildTransferBufferCreateInfo(VkDeviceSize const &bufferSize) {

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = bufferSize;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferCreateInfo.queueFamilyIndexCount = 1;
  bufferCreateInfo.pQueueFamilyIndices = &mQueueFamilyIndices.transfer.value();

  return bufferCreateInfo;
}

void VulkanMemoryManager::beginTransferCommandBuffer() const {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  validateVkResult(vkBeginCommandBuffer(mTransferCommandBuffer, &beginInfo));
}

void VulkanMemoryManager::endTransferCommandBuffer() const {
  validateVkResult(vkEndCommandBuffer(mTransferCommandBuffer));

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mTransferCommandBuffer;

  vkQueueSubmit(mTransferQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(mTransferQueue);
}

void VulkanMemoryManager::createStagingBuffer(VulkanBuffer &stagingBuffer,
                                              VkDeviceSize const &bufferSize) {
  VkBufferCreateInfo bufferCreateInfo = buildTransferBufferCreateInfo(bufferSize);
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &stagingBuffer.buffer, &stagingBuffer.allocation, nullptr));
}

void VulkanMemoryManager::copyBufferToBuffer(VulkanBuffer &srcBuffer, VulkanBuffer &dstBuffer,
                                             VkDeviceSize const &bufferSize,
                                             VkDeviceSize const srcOffset,
                                             VkDeviceSize const dstOffset) const {
  beginTransferCommandBuffer();

  VkBufferCopy copyRegion;
  copyRegion.srcOffset = srcOffset;
  copyRegion.dstOffset = dstOffset;
  copyRegion.size = bufferSize;

  vkCmdCopyBuffer(mTransferCommandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);

  endTransferCommandBuffer();
}

void VulkanMemoryManager::transferBufferOwnership(VkBuffer const &buffer,
                                                  uint32_t const srcQueueFamilyIndex,
                                                  uint32_t const dstQueueFamilyIndex) const {
  if (srcQueueFamilyIndex == dstQueueFamilyIndex) {
    return;
  }

  beginTransferCommandBuffer();

  VkBufferMemoryBarrier bufferMemoryBarrier{};
  bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  bufferMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
  bufferMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
  bufferMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
  bufferMemoryBarrier.buffer = buffer;
  bufferMemoryBarrier.size = VK_WHOLE_SIZE;

  vkCmdPipelineBarrier(mTransferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0,
                       nullptr);

  endTransferCommandBuffer();
}

void VulkanMemoryManager::initialize(VkInstance const &instance,
                                     VkPhysicalDevice const &physicalDevice, VkDevice const &device,
                                     VulkanQueues const &queues) {
  mPhysicalDevice = physicalDevice;
  mDevice = device;

  VmaAllocatorCreateInfo allocatorCreateInfo{};
  allocatorCreateInfo.instance = instance;
  allocatorCreateInfo.physicalDevice = physicalDevice;
  allocatorCreateInfo.device = device;

  validateVkResult(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));

  mTransferQueue = queues.transfer;
  mQueueFamilyIndices = queues.familyIndices;

  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags =
      VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = queues.familyIndices.transfer.value();

  validateVkResult(
      vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &mTransferCommandPool));

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandPool = mTransferCommandPool;
  commandBufferAllocateInfo.commandBufferCount = 1;

  validateVkResult(
      vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &mTransferCommandBuffer));
}

void VulkanMemoryManager::destroy() const {
  vkDestroyCommandPool(mDevice, mTransferCommandPool, nullptr);
  vmaDestroyAllocator(mAllocator);
}

void VulkanMemoryManager::destroyBuffer(VulkanBuffer const &buffer) const {
  vmaDestroyBuffer(mAllocator, buffer.buffer, buffer.allocation);
}

VulkanBuffer VulkanMemoryManager::buildMeshBuffer(Mesh const &mesh) {
  VulkanBuffer meshBuffer{*this};

  VkBufferCreateInfo bufferCreateInfo =
      buildTransferBufferCreateInfo(mesh.getIndicesSize() + mesh.getVerticesSize());
  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &meshBuffer.buffer, &meshBuffer.allocation, nullptr));

  updateMeshBuffer(meshBuffer, mesh);

  transferBufferOwnership(meshBuffer.buffer, mQueueFamilyIndices.transfer.value(),
                          mQueueFamilyIndices.graphics.value());

  return meshBuffer;
}

void VulkanMemoryManager::updateMeshBuffer(VulkanBuffer meshBuffer, Mesh const &mesh) {
  VkDeviceSize const indicesSize = mesh.getIndicesSize();
  VkDeviceSize const verticesSize = mesh.getVerticesSize();
  VkDeviceSize const meshDataSize = indicesSize + verticesSize;

  VulkanBuffer stagingBuffer{*this};
  createStagingBuffer(stagingBuffer, meshDataSize);
  copyDataToBuffer(mesh.getIndices().data(), stagingBuffer, indicesSize, 0, 0);
  copyDataToBuffer(mesh.getVertices().data(), stagingBuffer, verticesSize, 0, indicesSize);
  copyBufferToBuffer(stagingBuffer, meshBuffer, meshDataSize, 0, 0);
  destroyBuffer(stagingBuffer);
}

void VulkanMemoryManager::copyDataToBuffer(void const *srcData, VulkanBuffer &dstBuffer,
                                           VkDeviceSize const &dataSize,
                                           VkDeviceSize const &srcOffset,
                                           VkDeviceSize const &dstOffset) const {
  void *mappedMemory;
  vmaMapMemory(mAllocator, dstBuffer.allocation, &mappedMemory);
  std::memcpy(static_cast<char *>(mappedMemory) + dstOffset,
              static_cast<char const *>(srcData) + srcOffset, dataSize);
  vmaUnmapMemory(mAllocator, dstBuffer.allocation);
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
  vkDestroyImageView(mDevice, image.imageView, nullptr);
  vmaDestroyImage(mAllocator, image.image, image.allocation);
}

VulkanImage VulkanMemoryManager::createDepthBufferImage(VkExtent2D const &swapchainExtent) {
  return createImage(swapchainExtent, VulkanImage::getSupportedDepthBufferFormat(mPhysicalDevice),
                     VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                     VK_IMAGE_ASPECT_DEPTH_BIT);
}
void VulkanMemoryManager::createImageView(VulkanImage &image,
                                          VkImageAspectFlags const &imageAspect) {
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

  validateVkResult(vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &image.imageView));
}

} // namespace flex