#include "CommandBufferRecorder.hpp"

#include <array>
#include <stdexcept>

#include "Graphics/Utils/VulkanHelpers.hpp"

namespace cbl::gfx {
CommandBufferRecorder::CommandBufferRecorder(VkCommandBuffer &commandBuffer)
    : mCommandBuffer(commandBuffer) {}

CommandBufferRecorder &CommandBufferRecorder::begin() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  validateVkResult(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::beginOneTime() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  validateVkResult(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::copyBuffer(mem::Buffer const &src,
                                                         mem::Buffer const &dst) {
  if (!src.isValid || !dst.isValid) {
    throw std::runtime_error("Cannot copy data to/from an uninitialized buffer");
  }

  VkBufferCopy copyRegion{};
  // handle the possibility that one buffer is smaller than the other
  copyRegion.size = std::min(src.size, dst.size);

  vkCmdCopyBuffer(mCommandBuffer, src.buffer, dst.buffer, 1, &copyRegion);
  return *this;
}

CommandBufferRecorder &
CommandBufferRecorder::addMeshBufferMemoryBarrier(mem::Buffer const &buffer,
                                                  QueueFamilyIndices const &queueFamilyIndices) {

  VkBufferMemoryBarrier bufferMemoryBarrier{};
  bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  bufferMemoryBarrier.dstAccessMask = VK_ACCESS_INDEX_READ_BIT;
  bufferMemoryBarrier.srcQueueFamilyIndex = queueFamilyIndices.transfer;
  bufferMemoryBarrier.dstQueueFamilyIndex = queueFamilyIndices.graphics;
  bufferMemoryBarrier.buffer = buffer.buffer;
  bufferMemoryBarrier.size = VK_WHOLE_SIZE;

  vkCmdPipelineBarrier(mCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier,
                       0, nullptr);
  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::transitionImageLayout(
    mem::Image const &image, VkImageLayout const &oldLayout, VkImageLayout const &newLayout,
    QueueFamilyIndices const &queueFamilyIndices) {

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = queueFamilyIndices.transfer;
  barrier.dstQueueFamilyIndex = queueFamilyIndices.transfer;
  barrier.image = image.image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = 0;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(mCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1,
                       &barrier);

  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::copyBufferToImage(mem::Buffer const &src,
                                                                mem::Image const &dst) {
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {dst.extent.width, dst.extent.height, 1};

  vkCmdCopyBufferToImage(mCommandBuffer, src.buffer, dst.image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::setViewPort(VkExtent2D const &viewportExtent) {
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

CommandBufferRecorder &CommandBufferRecorder::setScissor(VkRect2D const &scissorRect) {
  vkCmdSetScissor(mCommandBuffer, 0, 1, &scissorRect);
  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::beginRenderPass(VkRenderPass const &renderPass,
                                                              VkFramebuffer const &framebuffer,
                                                              VkRect2D const &renderArea) {
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

CommandBufferRecorder &CommandBufferRecorder::pushCameraView(glm::mat4 const &view,
                                                             BaseShader const &shader) {
  vkCmdPushConstants(mCommandBuffer, shader.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                     sizeof(glm::mat4), &view);

  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::pushModelPosition(glm::mat4 const &position,
                                                                BaseShader const &shader) {
  vkCmdPushConstants(mCommandBuffer, shader.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
                     sizeof(glm::mat4), sizeof(glm::mat4), &position);

  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::bindGraphicsShader(BaseShader const &shader) {
  vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.pipeline);
  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::bindMaterial(BaseShader const &shader,
                                                           BaseMaterial const &material) {
  vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.pipelineLayout, 0,
                          static_cast<uint32_t>(material.descriptorSets.size()),
                          material.descriptorSets.data(), 0, nullptr);
  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::drawMesh(Mesh const &mesh) {
  if (!mesh.buffer.isValid) {
    return *this;
  }

  vkCmdBindIndexBuffer(mCommandBuffer, mesh.buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

  std::array<VkDeviceSize, 1> vertexBufferOffset{mesh.getIndicesSize()};
  vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, &mesh.buffer.buffer, vertexBufferOffset.data());

  vkCmdDrawIndexed(mCommandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::endRenderPass() {
  vkCmdEndRenderPass(mCommandBuffer);
  return *this;
}

CommandBufferRecorder &CommandBufferRecorder::end() {
  validateVkResult(vkEndCommandBuffer(mCommandBuffer));
  return *this;
}

void CommandBufferRecorder::submit(VkQueue const &submitQueue,
                                   VkFence const &fence = VK_NULL_HANDLE) {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mCommandBuffer;

  vkQueueSubmit(submitQueue, 1, &submitInfo, fence);
}

} // namespace cbl::gfx