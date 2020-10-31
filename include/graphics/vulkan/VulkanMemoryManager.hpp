#pragma once

#include "vk_mem_alloc.h"

#include "graphics/Mesh.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"
#include "graphics/vulkan/VulkanGPU.hpp"
#include "graphics/vulkan/VulkanImage.hpp"

namespace flex {
struct VulkanMemoryManager {
private:
  VulkanGPU mGPU{};
  VmaAllocator mAllocator{};

  VkCommandPool mTransferCommandPool{};
  VkCommandBuffer mTransferCommandBuffer{};

  VulkanBuffer createStagingBuffer(VkDeviceSize const &bufferSize);

  void destroyBufferOnFenceTrigger(VulkanBuffer buffer, VkFence fence) const;

public:
  VulkanMemoryManager() = default;

  void initialise(VulkanGPU const &gpu);
  void destroy() const;
  void destroyBuffer(VulkanBuffer const &buffer) const;

  VulkanBuffer createMeshBuffer(Mesh const &mesh);
  void updateMeshBuffer(VulkanBuffer meshBuffer, Mesh const &mesh);

  VulkanImage createImage(VkExtent2D const &extent, VkFormat const &format,
                          VkImageTiling const &tiling, VkImageUsageFlags const &usage,
                          VkImageAspectFlags const &imageAspect);
  void createImageView(VulkanImage &image, VkImageAspectFlags const &imageAspect) const;
  void destroyImage(VulkanImage &image);
};
} // namespace flex