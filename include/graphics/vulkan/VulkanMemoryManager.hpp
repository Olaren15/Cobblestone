#pragma once

#include <vector>

#include "vk_mem_alloc.h"

#include "graphics/Mesh.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"
#include "graphics/vulkan/VulkanQueues.hpp"

namespace flex {
struct VulkanMemoryManager {
private:
  VkDevice mDevice{};

  VmaAllocator mAllocator{};
  VkQueue mTransferQueue{};
  VulkanQueueFamilyIndices mQueueFamilyIndices;

  VkCommandPool mCommandPool{};
  VkCommandBuffer mTransferCommandBuffer{};

  VkBufferCreateInfo buildCommonBufferCreateInfo(VkDeviceSize const &bufferSize);

  void beginTransferCommandBuffer() const;
  void endTransferCommandBuffer() const;

public:
  VulkanMemoryManager() = default;
  VulkanMemoryManager(VulkanMemoryManager const &) = delete;

  void initialize(VkInstance const &instance, VkPhysicalDevice const &physicalDevice,
                  VkDevice const &device, VulkanQueues const &queues);
  void destroy() const;

  void createMeshBuffer(VulkanBuffer &meshBuffer, Mesh &mesh);
  void createStagingBuffer(VulkanBuffer &stagingBuffer, void *data, VkDeviceSize const &dataSize);

  void copyBuffer(VulkanBuffer &srcBuffer, VulkanBuffer &dstBuffer, VkDeviceSize const &bufferSize,
                  VkDeviceSize srcOffset, VkDeviceSize dstOffset) const;

  void transferBufferOwnership(uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex,
                               VkBuffer const &buffer) const;

  void destroyBuffer(VulkanBuffer const &buffer) const;
};
} // namespace flex