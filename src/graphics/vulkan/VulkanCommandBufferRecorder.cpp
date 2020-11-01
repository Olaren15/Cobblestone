#include "graphics/vulkan/VulkanCommandBufferRecorder.hpp"

#include <array>
#include <stdexcept>

#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {
VulkanCommandBufferRecorder::VulkanCommandBufferRecorder(VkCommandBuffer &commandBuffer)
    : mCommandBuffer(commandBuffer) {}

VulkanCommandBufferRecorder &VulkanCommandBufferRecorder::begin() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  validateVkResult(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
  return *this;
}

VulkanCommandBufferRecorder &VulkanCommandBufferRecorder::beginOneTime() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  validateVkResult(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
  return *this;
}

VulkanCommandBufferRecorder &VulkanCommandBufferRecorder::copyBuffer(VulkanBuffer const &src,
                                                                     VulkanBuffer const &dst) {
  if (!src.isValid || !dst.isValid) {
    throw std::runtime_error("Cannot copy data to/from an uninitialized buffer");
  }

  VkBufferCopy copyRegion{};
  // handle the possibility that one buffer is smaller than the other
  copyRegion.size = std::min(src.size, dst.size);

  vkCmdCopyBuffer(mCommandBuffer, src.buffer, dst.buffer, 1, &copyRegion);
  return *this;
}

VulkanCommandBufferRecorder &VulkanCommandBufferRecorder::addStagingBufferMemoryBarrier(
    VulkanBuffer const &buffer, VulkanQueueFamilyIndices const &queueFamilyIndices) {

  VkBufferMemoryBarrier bufferMemoryBarrier{};
  bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  bufferMemoryBarrier.dstAccessMask = 0;
  bufferMemoryBarrier.srcQueueFamilyIndex = queueFamilyIndices.transfer.value();
  bufferMemoryBarrier.dstQueueFamilyIndex = queueFamilyIndices.graphics.value();
  bufferMemoryBarrier.buffer = buffer.buffer;
  bufferMemoryBarrier.size = VK_WHOLE_SIZE;

  vkCmdPipelineBarrier(mCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier,
                       0, nullptr);
  return *this;
}

VulkanCommandBufferRecorder &
VulkanCommandBufferRecorder::setViewPort(VkExtent2D const &viewportExtent) {
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(viewportExtent.width);
  viewport.height = static_cast<float>(viewportExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);

  return *this;
}

VulkanCommandBufferRecorder &VulkanCommandBufferRecorder::setScissor(VkRect2D const &scissorRect) {
  vkCmdSetScissor(mCommandBuffer, 0, 1, &scissorRect);
  return *this;
}

VulkanCommandBufferRecorder &VulkanCommandBufferRecorder::beginRenderPass(
    VkRenderPass const &renderPass, VkFramebuffer const &framebuffer, VkRect2D const &renderArea) {
  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = renderPass;
  renderPassBeginInfo.framebuffer = framebuffer;
  renderPassBeginInfo.renderArea = renderArea;
  renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassBeginInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

  return *this;
}

VulkanCommandBufferRecorder &
VulkanCommandBufferRecorder::pushConstants(void const *data, VkDeviceSize const &dataSize,
                                           VkPipelineLayout const &layout,
                                           VkShaderStageFlags const &shaderStage) {
  vkCmdPushConstants(mCommandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, dataSize, data);

  return *this;
}

VulkanCommandBufferRecorder &
VulkanCommandBufferRecorder::bindPipeline(const VkPipeline &pipeline,
                                          const VkPipelineBindPoint &bindPoint) {

  vkCmdBindPipeline(mCommandBuffer, bindPoint, pipeline);
  return *this;
}

VulkanCommandBufferRecorder &
VulkanCommandBufferRecorder::drawMeshes(std::vector<Mesh> const &meshes) {
  for (Mesh const &mesh : meshes) {
    if (!mesh.buffer.isValid) {
      throw std::runtime_error("Cannot draw without allocated vram");
    }

    vkCmdBindIndexBuffer(mCommandBuffer, mesh.buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

    std::array<VkDeviceSize, 1> vertexBufferOffset{mesh.getIndicesSize()};
    vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, &mesh.buffer.buffer, vertexBufferOffset.data());

    vkCmdDrawIndexed(mCommandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
  }
  return *this;
}

VulkanCommandBufferRecorder &VulkanCommandBufferRecorder::endRenderPass() {
  vkCmdEndRenderPass(mCommandBuffer);
  return *this;
}

VulkanCommandBufferRecorder &VulkanCommandBufferRecorder::end() {
  validateVkResult(vkEndCommandBuffer(mCommandBuffer));
  return *this;
}

void VulkanCommandBufferRecorder::submit(VkQueue const &submitQueue) {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBuffer;

  vkQueueSubmit(submitQueue, 1, &submitInfo, VK_NULL_HANDLE);
}

void VulkanCommandBufferRecorder::submitWithFence(VkQueue const &submitQueue,
                                                  VkFence const &fence) {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBuffer;

  vkQueueSubmit(submitQueue, 1, &submitInfo, fence);
}

} // namespace flex