#pragma once

#include <filesystem>

#include "External/vk_mem_alloc/vk_mem_alloc.h"

#include "Graphics/GPU/GPU.hpp"
#include "Graphics/Memory/Buffer/Buffer.hpp"
#include "Graphics/Memory/Image/Image.hpp"
#include "Graphics/Memory/Texture/Texture.hpp"
#include "Graphics/Mesh/Mesh.hpp"

namespace cbl::gfx::mem {
struct MemoryManager {
private:
  GPU const &mGPU;
  VmaAllocator mAllocator{};

  VkCommandPool mCommandPool{};
  VkCommandBuffer mCommandBuffer{};

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

  [[nodiscard]] Texture createTexture(std::filesystem::path const &texturePath);
  void destroyTexture(Texture &texture);

  [[nodiscard]] Image createImage(VkExtent2D const &extent, VkFormat const &format,
                                  VkImageTiling const &tiling, VkImageUsageFlags const &usage,
                                  VkImageAspectFlags const &imageAspect);
  void createImageView(Image &image, VkImageAspectFlags const &imageAspect) const;
  void destroyImage(Image &image);
};
} // namespace flex