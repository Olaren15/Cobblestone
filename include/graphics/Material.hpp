#pragma once

#include <vulkan/vulkan.h>

#include "graphics/GPU.hpp"
#include "graphics/MemoryManager.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace flex {
struct Material {
private:
  MemoryManager &mMemoryManager;

public:
  VkDescriptorSet descriptorSet{};
  Texture texture{};

  Material() = delete;
  explicit Material(GPU const &gpu, MemoryManager &memoryManager, Shader const &shader);
  ~Material();
};
} // namespace flex