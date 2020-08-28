#include "graphics/vulkan/VulkanMemoryManager.hpp"

#include <memory>
#include <vector>

#include "graphics/vulkan/VulkanHelpers.hpp"

VkBufferCreateInfo
flex::VulkanMemoryManager::buildCommonBufferCreateInfo(VkDeviceSize const &bufferSize) {

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = bufferSize;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferCreateInfo.queueFamilyIndexCount = 1;
  bufferCreateInfo.pQueueFamilyIndices = &mQueueFamilyIndices.transfer.value();

  return bufferCreateInfo;
}

void flex::VulkanMemoryManager::beginTransferCommandBuffer() const {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  validateVkResult(vkBeginCommandBuffer(mTransferCommandBuffer, &beginInfo));
}

void flex::VulkanMemoryManager::endTransferCommandBuffer() const {
  validateVkResult(vkEndCommandBuffer(mTransferCommandBuffer));

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mTransferCommandBuffer;

  vkQueueSubmit(mTransferQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(mTransferQueue);
}

void flex::VulkanMemoryManager::initialize(VkInstance const &instance,
                                           VkPhysicalDevice const &physicalDevice,
                                           VkDevice const &device,
                                           flex::VulkanQueues const &queues) {
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

  validateVkResult(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &mCommandPool));

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandPool = mCommandPool;
  commandBufferAllocateInfo.commandBufferCount = 1;

  validateVkResult(
      vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &mTransferCommandBuffer));
}

void flex::VulkanMemoryManager::destroy() const {
  vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
  vmaDestroyAllocator(mAllocator);
}

void flex::VulkanMemoryManager::createMeshBuffer(VulkanBuffer &meshBuffer, Mesh &mesh) {

  VkDeviceSize const indicesSize = mesh.getIndicesSize();
  VkDeviceSize const verticesSize = mesh.getVerticesSize();
  VkDeviceSize const meshDataSize = indicesSize + verticesSize;

  VkBufferCreateInfo bufferCreateInfo = buildCommonBufferCreateInfo(meshDataSize);
  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &meshBuffer.buffer, &meshBuffer.allocation, nullptr));

  VulkanBuffer stagingBuffer;
  createStagingBuffer(stagingBuffer, mesh.indices.data(), indicesSize);
  copyBuffer(stagingBuffer, meshBuffer, indicesSize, 0, 0);
  destroyBuffer(stagingBuffer);

  createStagingBuffer(stagingBuffer, mesh.vertices.data(), verticesSize);
  copyBuffer(stagingBuffer, meshBuffer, verticesSize, 0, indicesSize);
  destroyBuffer(stagingBuffer);

  transferBufferOwnership(mQueueFamilyIndices.transfer.value(),
                          mQueueFamilyIndices.graphics.value(), meshBuffer.buffer);
}

void flex::VulkanMemoryManager::createStagingBuffer(VulkanBuffer &stagingBuffer, void *data,
                                                    VkDeviceSize const &dataSize) {
  VkBufferCreateInfo bufferCreateInfo = buildCommonBufferCreateInfo(dataSize);
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &stagingBuffer.buffer, &stagingBuffer.allocation, nullptr));

  void *mappedMemory;
  vmaMapMemory(mAllocator, stagingBuffer.allocation, &mappedMemory);
  std::memcpy(mappedMemory, data, dataSize);
  vmaUnmapMemory(mAllocator, stagingBuffer.allocation);
}

void flex::VulkanMemoryManager::copyBuffer(VulkanBuffer &srcBuffer, VulkanBuffer &dstBuffer,
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

void flex::VulkanMemoryManager::transferBufferOwnership(uint32_t const srcQueueFamilyIndex,
                                                        uint32_t const dstQueueFamilyIndex,
                                                        VkBuffer const &buffer) const {

  beginTransferCommandBuffer();

  VkBufferMemoryBarrier bufferMemoryBarrier{};
  bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  bufferMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  bufferMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
  bufferMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
  bufferMemoryBarrier.buffer = buffer;
  bufferMemoryBarrier.size = VK_WHOLE_SIZE;

  vkCmdPipelineBarrier(mTransferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0,
                       nullptr);

  endTransferCommandBuffer();
}

void flex::VulkanMemoryManager::destroyBuffer(VulkanBuffer const &buffer) const {
  vmaDestroyBuffer(mAllocator, buffer.buffer, buffer.allocation);
}
