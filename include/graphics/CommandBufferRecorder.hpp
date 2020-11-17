#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Buffer.hpp"
#include "QueueFamiliIndices.hpp"
#include "graphics/Mesh.hpp"

namespace flex {
struct CommandBufferRecorder {
private:
  VkCommandBuffer &mCommandBuffer;

public:
  explicit CommandBufferRecorder(VkCommandBuffer &commandBuffer);

  CommandBufferRecorder &begin();
  CommandBufferRecorder &beginOneTime();

  CommandBufferRecorder &copyBuffer(Buffer const &src, Buffer const &dst);
  CommandBufferRecorder &
  addStagingBufferMemoryBarrier(Buffer const &buffer, QueueFamiliIndices const &queueFamilyIndices);

  CommandBufferRecorder &setViewPort(VkExtent2D const &viewportExtent);
  CommandBufferRecorder &setScissor(VkRect2D const &scissorRect);
  CommandBufferRecorder &beginRenderPass(VkRenderPass const &renderPass,
                                               VkFramebuffer const &frameBuffer,
                                               VkRect2D const &renderArea);
  CommandBufferRecorder &pushCameraView(glm::mat4 const &view,
                                              VkPipelineLayout const &layout);
  CommandBufferRecorder &pushModelPosition(glm::mat4 const &position,
                                                 VkPipelineLayout const &layout);
  CommandBufferRecorder &bindPipeline(VkPipeline const &pipeline,
                                            VkPipelineBindPoint const &bindPoint);
  CommandBufferRecorder &drawMesh(Mesh const &mesh);
  CommandBufferRecorder &drawMeshes(std::vector<Mesh> const &meshes);
  CommandBufferRecorder &endRenderPass();

  CommandBufferRecorder &end();
  void submit(VkQueue const &submitQueue);
  void submitWithFence(VkQueue const &submitQueue, VkFence const &fence);
};
} // namespace flex