#include "graphics/vulkan/VulkanBuffer.hpp"

#include <memory>

#include "graphics/vulkan/VulkanHelpers.hpp"

namespace flex {
void VulkanBuffer::allocateVertex(VmaAllocator const &allocator, size_t const &bufferSize) {
  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = bufferSize;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  validateVkResult(vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer,
                                   &allocation, &allocationInfo));
}

void VulkanBuffer::setContent(VmaAllocator const &allocator, void *data) const {
  void *mappedMemory;
  validateVkResult(vmaMapMemory(allocator, allocation, &mappedMemory));
  std::memcpy(mappedMemory, data, allocationInfo.size);
  vmaUnmapMemory(allocator, allocation);
}

void VulkanBuffer::destroy(VmaAllocator const &allocator) const {
  vmaDestroyBuffer(allocator, buffer, allocation);
}
} // namespace flex
