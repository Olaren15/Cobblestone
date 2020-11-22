#pragma once

#include <string>

#include <vulkan/vulkan.h>

#include "Graphics/GPU/GPU.hpp"
#include "Graphics/Memory/MemoryManager/MemoryManager.hpp"
#include "Graphics/Memory/Texture/Texture.hpp"
#include "Graphics/Shaders/BaseShader.hpp"

namespace cbl::gfx {
struct BaseMaterial {
protected:
  mem::MemoryManager &mMemoryManager;

public:
  std::vector<VkDescriptorSet> descriptorSets;

  BaseMaterial() = delete;
  explicit BaseMaterial(GPU const &gpu, mem::MemoryManager &memoryManager,
                        BaseShader const *shader);
  virtual ~BaseMaterial() = default;
};
} // namespace cbl::gfx