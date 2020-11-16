#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/Mesh.hpp"
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

  VulkanCommandBufferRecorder &setViewPort(VkExtent2D const &viewportExtent);
  VulkanCommandBufferRecorder &setScissor(VkRect2D const &scissorRect);
  VulkanCommandBufferRecorder &beginRenderPass(VkRenderPass const &renderPass,
                                               VkFramebuffer const &frameBuffer,
                                               VkRect2D const &renderArea);
  VulkanCommandBufferRecorder &pushConstants(void const *data, VkDeviceSize const &dataSize,
                                             VkPipelineLayout const &layout,
                                             VkShaderStageFlags const &shaderStage);
  VulkanCommandBufferRecorder &bindPipeline(VkPipeline const &pipeline,
                                            VkPipelineBindPoint const &bindPoint);
  VulkanCommandBufferRecorder &drawMesh(Mesh const &mesh);
  VulkanCommandBufferRecorder &drawMeshes(std::vector<Mesh> const &meshes);
  VulkanCommandBufferRecorder &endRenderPass();

  VulkanCommandBufferRecorder &end();
  void submit(VkQueue const &submitQueue);
  void submitWithFence(VkQueue const &submitQueue, VkFence const &fence);
};
} // namespace flex