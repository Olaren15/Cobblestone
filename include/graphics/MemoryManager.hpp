#pragma once

#include "vk_mem_alloc.h"

#include "Buffer.hpp"
#include "GPU.hpp"
#include "Image.hpp"
#include "graphics/Mesh.hpp"

namespace flex {
struct MemoryManager {
private:
  GPU const &mGPU;
  VmaAllocator mAllocator{};

  VkCommandPool mTransferCommandPool{};
  VkCommandBuffer mTransferCommandBuffer{};

  void allocateBuffer(VkBufferCreateInfo const &bufferInfo,
                      VmaAllocationCreateInfo const &allocInfo, Buffer &buffer);
  Buffer createStagingBuffer(VkDeviceSize const &bufferSize);

  void destroyBufferOnFenceTrigger(Buffer buffer, VkFence fence) const;

public:
  MemoryManager() = delete;
  explicit MemoryManager(GPU const &gpu);
  ~MemoryManager();

  void destroyBuffer(Buffer &buffer) const;

  void generateMeshBuffer(Mesh &mesh);
  void updateMeshBuffer(Mesh &mesh);

  Image createImage(VkExtent2D const &extent, VkFormat const &format, VkImageTiling const &tiling,
                    VkImageUsageFlags const &usage, VkImageAspectFlags const &imageAspect);
  void createImageView(Image &image, VkImageAspectFlags const &imageAspect) const;
  void destroyImage(Image &image);
};
} // namespace flex