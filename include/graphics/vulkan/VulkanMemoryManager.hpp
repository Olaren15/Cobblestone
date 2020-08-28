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
  std::vector<uint32_t> mBufferSharedQueues;
  VkCommandPool mCommandPool{};

  VkBufferCreateInfo buildCommonBufferCreateInfo(VkDeviceSize const &bufferSize);

public:
  VulkanMemoryManager() = default;
  VulkanMemoryManager(VulkanMemoryManager const &) = delete;

  void initialize(VkInstance const &instance, VkPhysicalDevice const &physicalDevice,
                  VkDevice const &device, VulkanQueues const &queues);
  void destroy() const;

  void createMeshBuffer(VulkanBuffer &vertexBuffer, Mesh &mesh);
  void createStagingBuffer(VulkanBuffer &stagingBuffer, void *data, VkDeviceSize const &dataSize);

  void copyBuffer(VulkanBuffer &srcBuffer, VulkanBuffer &dstBuffer, VkDeviceSize const &bufferSize,
                  VkDeviceSize const srcOffset, VkDeviceSize const dstOffset) const;

  void destroyBuffer(VulkanBuffer const &buffer) const;
};
} // namespace flex