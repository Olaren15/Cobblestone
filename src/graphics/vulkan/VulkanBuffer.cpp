#include "graphics/vulkan/VulkanBuffer.hpp"

#include "graphics/vulkan/VulkanMemoryManager.hpp"

namespace flex {
VulkanBuffer::VulkanBuffer(VulkanMemoryManager &manager) : memoryManager(manager) {}
VulkanBuffer &VulkanBuffer::operator=(VulkanBuffer other) {
  allocation = other.allocation;
  buffer = other.buffer;
  memoryManager = other.memoryManager;
  return *this;
}
} // namespace flex