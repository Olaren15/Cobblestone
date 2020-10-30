#include "graphics/vulkan/VulkanMemoryManager.hpp"

#include <cstring>
#include <thread>

#include "graphics/vulkan/VulkanCommandBufferRecorder.hpp"
#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {

VulkanBuffer VulkanMemoryManager::createStagingBuffer(VkDeviceSize const &bufferSize) {
  VulkanBuffer stagingBuffer{*this};

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = bufferSize;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferCreateInfo.queueFamilyIndexCount = 1;
  bufferCreateInfo.pQueueFamilyIndices = &mQueueFamilyIndices.transfer.value();
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &stagingBuffer.buffer, &stagingBuffer.allocation, nullptr));
  stagingBuffer.size = bufferCreateInfo.size;

  return stagingBuffer;
}

void VulkanMemoryManager::destroyBufferOnFenceTrigger(VulkanBuffer buffer, VkFence fence) {
  vkWaitForFences(mDevice, 1, &fence, VK_TRUE, UINT64_MAX);
  vkDestroyFence(mDevice, fence, nullptr);
  destroyBuffer(buffer);
}

void VulkanMemoryManager::initialize(VkInstance const &instance,
                                     VkPhysicalDevice const &physicalDevice, VkDevice const &device,
                                     VulkanQueues const &queues) {
  mPhysicalDevice = physicalDevice;
  mDevice = device;
  mTransferQueue = queues.transfer;
  mQueueFamilyIndices = queues.familyIndices;

  VmaAllocatorCreateInfo allocatorCreateInfo{};
  allocatorCreateInfo.instance = instance;
  allocatorCreateInfo.physicalDevice = physicalDevice;
  allocatorCreateInfo.device = device;

  validateVkResult(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));

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

VulkanBuffer VulkanMemoryManager::createMeshBuffer(Mesh const &mesh) {

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = mesh.getRequiredBufferSize();
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferCreateInfo.queueFamilyIndexCount = 1;
  bufferCreateInfo.pQueueFamilyIndices = &mQueueFamilyIndices.transfer.value();
  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VulkanBuffer meshBuffer{*this};
  validateVkResult(vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocationCreateInfo,
                                   &meshBuffer.buffer, &meshBuffer.allocation, nullptr));
  meshBuffer.size = bufferCreateInfo.size;

  updateMeshBuffer(meshBuffer, mesh);

  return meshBuffer;
}

void VulkanMemoryManager::updateMeshBuffer(VulkanBuffer meshBuffer, Mesh const &mesh) {
  VulkanBuffer stagingBuffer = createStagingBuffer(mesh.getRequiredBufferSize());

  void *mappedMemory;
  vmaMapMemory(mAllocator, stagingBuffer.allocation, &mappedMemory);
  std::memcpy(mappedMemory, mesh.indices.data(), mesh.getIndicesSize());
  std::memcpy(static_cast<char *>(mappedMemory) + mesh.getIndicesSize(), mesh.vertices.data(),
              mesh.getVerticesSize());
  vmaUnmapMemory(mAllocator, stagingBuffer.allocation);

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  VkFence bufferCopiedFence{};
  validateVkResult(vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &bufferCopiedFence));

  VulkanCommandBufferRecorder recorder{mTransferCommandBuffer};
  recorder.beginOneTime()
      .copyBuffer(stagingBuffer, meshBuffer)
      .addStagingBufferMemoryBarrier(meshBuffer, mQueueFamilyIndices)
      .end()
      .submitWithFence(mTransferQueue, bufferCopiedFence);

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
  vkDestroyImageView(mDevice, image.imageView, nullptr);
  vmaDestroyImage(mAllocator, image.image, image.allocation);
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