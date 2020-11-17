#include "graphics/Frame.hpp"

namespace flex {
void Frame::initialise(GPU const &gpu) {
  // command pool
  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  commandPoolCreateInfo.queueFamilyIndex = gpu.queueFamilyIndices.graphics.value();

  validateVkResult(vkCreateCommandPool(gpu.device, &commandPoolCreateInfo, nullptr, &commandPool));

  // command buffers
  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = 1;

  validateVkResult(
      vkAllocateCommandBuffers(gpu.device, &commandBufferAllocateInfo, &commandBuffer));

  // sync objects
  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  validateVkResult(
      vkCreateSemaphore(gpu.device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore));
  validateVkResult(
      vkCreateSemaphore(gpu.device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  validateVkResult(vkCreateFence(gpu.device, &fenceCreateInfo, nullptr, &renderFinishedFence));
}

void Frame::destroy(GPU const &gpu) const {
  vkDestroySemaphore(gpu.device, imageAvailableSemaphore, nullptr);
  vkDestroySemaphore(gpu.device, renderFinishedSemaphore, nullptr);
  vkDestroyFence(gpu.device, renderFinishedFence, nullptr);
  vkDestroyCommandPool(gpu.device, commandPool, nullptr);
}
} // namespace flex