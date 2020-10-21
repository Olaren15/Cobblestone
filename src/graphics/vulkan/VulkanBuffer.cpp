#include "graphics/vulkan/VulkanBuffer.hpp"

#include "graphics/vulkan/VulkanMemoryManager.hpp"

namespace flex {
VulkanBuffer::VulkanBuffer(VulkanMemoryManager *manager) { memoryManager = manager; }
} // namespace flex