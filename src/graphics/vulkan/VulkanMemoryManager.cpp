#include "graphics/vulkan/VulkanMemoryManager.hpp"

#include <memory>
#include <vector>

#include "graphics/vulkan/VulkanHelpers.hpp"

VkBufferCreateInfo
flex::VulkanMemoryManager::buildCommonBufferCreateInfo(VkDeviceSize const &bufferSize) {

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = bufferSize;
  if (mBufferSharedQueues.size() > 1)
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
  else
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(mBufferSharedQueues.size());
  bufferCreateInfo.pQueueFamilyIndices = mBufferSharedQueues.data();

  return bufferCreateInfo;
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

  mBufferSharedQueues.push_back(queues.familyIndices.graphics.value());
  if (queues.familyIndices.hasUniqueTransferQueue()) {
    mBufferSharedQueues.push_back(queues.familyIndices.transfer.value());
  }

  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  commandPoolCreateInfo.queueFamilyIndex = queues.familyIndices.transfer.value();

  validateVkResult(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &mCommandPool));
}

void flex::VulkanMemoryManager::destroy() const {
  vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
  vmaDestroyAllocator(mAllocator);
}

void flex::VulkanMemoryManager::createVertexBuffer(VulkanBuffer &vertexBuffer, void *data,
                                                   VkDeviceSize const &dataSize) {
  VkBufferCreateInfo bufferCreateInfo = buildCommonBufferCreateInfo(dataSize);
  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &vertexBuffer.buffer, &vertexBuffer.allocation,
                                   &vertexBuffer.allocationInfo));

  VulkanBuffer stagingBuffer;
  createStagingBuffer(stagingBuffer, data, dataSize);
  copyBuffer(stagingBuffer, vertexBuffer, dataSize);
  destroyBuffer(stagingBuffer);
}

void flex::VulkanMemoryManager::createStagingBuffer(VulkanBuffer &stagingBuffer, void *data,
                                                    VkDeviceSize const &dataSize) {
  VkBufferCreateInfo bufferCreateInfo = buildCommonBufferCreateInfo(dataSize);
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &stagingBuffer.buffer, &stagingBuffer.allocation,
                                   &stagingBuffer.allocationInfo));

  void *mappedMemory;
  vmaMapMemory(mAllocator, stagingBuffer.allocation, &mappedMemory);
  std::memcpy(mappedMemory, data, dataSize);
  vmaUnmapMemory(mAllocator, stagingBuffer.allocation);
}

void flex::VulkanMemoryManager::copyBuffer(VulkanBuffer &srcBuffer, VulkanBuffer &dstBuffer,
                                           VkDeviceSize const &bufferSize) const {

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandPool = mCommandPool;
  commandBufferAllocateInfo.commandBufferCount = 1;

  VkCommandBuffer transferCommandBuffer;
  validateVkResult(
      vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &transferCommandBuffer));

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  validateVkResult(vkBeginCommandBuffer(transferCommandBuffer, &beginInfo));

  VkBufferCopy copyRegion;
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = bufferSize;

  vkCmdCopyBuffer(transferCommandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);

  validateVkResult(vkEndCommandBuffer(transferCommandBuffer));

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &transferCommandBuffer;

  vkQueueSubmit(mTransferQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(mTransferQueue);

  vkFreeCommandBuffers(mDevice, mCommandPool, 1, &transferCommandBuffer);
}

void flex::VulkanMemoryManager::destroyBuffer(VulkanBuffer const &buffer) const {
  vmaDestroyBuffer(mAllocator, buffer.buffer, buffer.allocation);
}
