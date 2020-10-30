#pragma once

#include "vk_mem_alloc.h"

#include "graphics/Mesh.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"
#include "graphics/vulkan/VulkanImage.hpp"
#include "graphics/vulkan/VulkanQueues.hpp"

namespace flex {
struct VulkanMemoryManager {
private:
  VkPhysicalDevice mPhysicalDevice{};
  VkDevice mDevice{};

  VmaAllocator mAllocator{};
  VkQueue mTransferQueue{};
  VulkanQueueFamilyIndices mQueueFamilyIndices;

  VkCommandPool mTransferCommandPool{};
  VkCommandBuffer mTransferCommandBuffer{};

  VulkanBuffer createStagingBuffer(VkDeviceSize const &bufferSize);

  void destroyBufferOnFenceTrigger(VulkanBuffer buffer, VkFence fence);

public:
  VulkanMemoryManager() = default;

  void initialize(VkInstance const &instance, VkPhysicalDevice const &physicalDevice,
                  VkDevice const &device, VulkanQueues const &queues);
  void destroy() const;
  void destroyBuffer(VulkanBuffer const &buffer) const;

  VulkanBuffer createMeshBuffer(Mesh const &mesh);
  void updateMeshBuffer(VulkanBuffer meshBuffer, Mesh const &mesh);

  VulkanImage createImage(VkExtent2D const &extent, VkFormat const &format,
                          VkImageTiling const &tiling, VkImageUsageFlags const &usage,
                          VkImageAspectFlags const &imageAspect);
  void createImageView(VulkanImage &image, VkImageAspectFlags const &imageAspect);
  void destroyImage(VulkanImage &image);
};
} // namespace flex