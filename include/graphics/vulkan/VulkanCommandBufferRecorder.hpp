#pragma once

#include <vulkan/vulkan.h>

#include "graphics/vulkan/VulkanBuffer.hpp"
#include "graphics/vulkan/VulkanQueueFamilyIndices.hpp"

namespace flex {
struct VulkanCommandBufferRecorder {
private:
  VkCommandBuffer &mCommandBuffer;

public:
  explicit VulkanCommandBufferRecorder(VkCommandBuffer &commandBuffer);

  VulkanCommandBufferRecorder &begin();
  VulkanCommandBufferRecorder &beginOneTime();
  VulkanCommandBufferRecorder &copyBuffer(VulkanBuffer const &src, VulkanBuffer const &dst);
  VulkanCommandBufferRecorder &
  addStagingBufferMemoryBarrier(VulkanBuffer const &buffer,
                                VulkanQueueFamilyIndices const &queueFamilyIndices);

  VulkanCommandBufferRecorder &end();
  void submit(VkQueue const &submitQueue);
  void submitWithFence(VkQueue const &submitQueue, VkFence const &fence);
};
} // namespace flex