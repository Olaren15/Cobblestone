#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Graphics/Materials/BaseMaterial.hpp"
#include "Graphics/Memory/Buffer/Buffer.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Shaders/BaseShader.hpp"
#include "Graphics/Swapchain/Swapchain.hpp"

namespace cbl::gfx {
struct CommandBufferRecorder {
private:
  VkCommandBuffer &mCommandBuffer;

public:
  explicit CommandBufferRecorder(VkCommandBuffer &commandBuffer);

  CommandBufferRecorder &begin();
  CommandBufferRecorder &beginOneTime();

  CommandBufferRecorder &copyBuffer(mem::Buffer const &src, mem::Buffer const &dst);
  CommandBufferRecorder &addMeshBufferMemoryBarrier(mem::Buffer const &buffer,
                                                    QueueFamilyIndices const &queueFamilyIndices);

  CommandBufferRecorder &transitionImageLayout(mem::Image const &image,
                                               VkImageLayout const &oldLayout,
                                               VkImageLayout const &newLayout,
                                               QueueFamilyIndices const &queueFamilyIndices);
  CommandBufferRecorder &copyBufferToImage(mem::Buffer const &src, mem::Image const &dst);

  CommandBufferRecorder &setViewPort(VkExtent2D const &viewportExtent);
  CommandBufferRecorder &setScissor(VkRect2D const &scissorRect);
  CommandBufferRecorder &beginRenderPass(VkRenderPass const &renderPass,
                                         VkFramebuffer const &frameBuffer,
                                         VkRect2D const &renderArea);
  CommandBufferRecorder &pushCameraView(glm::mat4 const &view, BaseShader const &shader);
  CommandBufferRecorder &pushModelPosition(glm::mat4 const &position, BaseShader const &shader);
  CommandBufferRecorder &bindGraphicsShader(BaseShader const &shader);
  CommandBufferRecorder &drawMesh(Mesh const &mesh);
  CommandBufferRecorder &bindMaterial(BaseShader const &shader, BaseMaterial const &material);
  CommandBufferRecorder &drawMeshes(std::vector<Mesh> const &meshes);
  CommandBufferRecorder &endRenderPass();

  CommandBufferRecorder &end();
  void submit(VkQueue const &submitQueue);
  void submitWithFence(VkQueue const &submitQueue, VkFence const &fence);
};
} // namespace cbl::gfx