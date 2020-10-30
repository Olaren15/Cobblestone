#include "graphics/vulkan/VulkanCommandBufferRecorder.hpp"

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