#include "graphics/vulkan/VulkanMemoryManager.hpp"

#include <memory>
#include <vector>

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

void VulkanMemoryManager::initialize(VkInstance const &instance,
                                     VkPhysicalDevice const &physicalDevice, VkDevice const &device,
                                     VulkanQueues const &queues) {
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

void VulkanMemoryManager::createMeshBuffer(VulkanBuffer &meshBuffer, Mesh &mesh) {

  VkDeviceSize const indicesSize = mesh.getIndicesSize();
  VkDeviceSize const verticesSize = mesh.getVerticesSize();
  VkDeviceSize const meshDataSize = indicesSize + verticesSize;

  VkBufferCreateInfo bufferCreateInfo = buildTransferBufferCreateInfo(meshDataSize);
  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &meshBuffer.buffer, &meshBuffer.allocation, nullptr));

  VulkanBuffer stagingBuffer;
  createStagingBuffer(stagingBuffer, meshDataSize);
  copyDataToBuffer(mesh.indices.data(), stagingBuffer, indicesSize, 0, 0);
  copyDataToBuffer(mesh.vertices.data(), stagingBuffer, verticesSize, 0, indicesSize);
  copyBufferToBuffer(stagingBuffer, meshBuffer, meshDataSize, 0, 0);
  destroyBuffer(stagingBuffer);

  transferBufferOwnership(meshBuffer.buffer, mQueueFamilyIndices.transfer.value(),
                          mQueueFamilyIndices.graphics.value());
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

void VulkanMemoryManager::copyDataToBuffer(void *srcData, VulkanBuffer &dstBuffer,
                                           VkDeviceSize const &dataSize,
                                           VkDeviceSize const &srcOffset,
                                           VkDeviceSize const &dstOffset) const {
  void *mappedMemory;
  vmaMapMemory(mAllocator, dstBuffer.allocation, &mappedMemory);
  std::memcpy(static_cast<char *>(mappedMemory) + dstOffset,
              static_cast<char *>(srcData) + srcOffset, dataSize);
  vmaUnmapMemory(mAllocator, dstBuffer.allocation);
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

void VulkanMemoryManager::destroyBuffer(VulkanBuffer const &buffer) const {
  vmaDestroyBuffer(mAllocator, buffer.buffer, buffer.allocation);
}
} // namespace flex