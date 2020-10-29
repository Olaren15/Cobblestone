#include "graphics/vulkan/VulkanFrame.hpp"

namespace flex {
void VulkanFrame::initialise(VkDevice const &device, VulkanQueues const &queues) {
  // command pool
  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  commandPoolCreateInfo.queueFamilyIndex = queues.familyIndices.graphics.value();

  validateVkResult(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));

  // command buffers
  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = 1;

  validateVkResult(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer));

  // sync objects
  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  validateVkResult(
      vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore));
  validateVkResult(
      vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  validateVkResult(vkCreateFence(device, &fenceCreateInfo, nullptr, &renderFinishedFence));
}

void VulkanFrame::destroy(const VkDevice &device) {
  vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
  vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
  vkDestroyFence(device, renderFinishedFence, nullptr);
  vkDestroyCommandPool(device, commandPool, nullptr);
}
} // namespace flex