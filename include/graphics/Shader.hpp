#pragma once

#include <filesystem>

#include <fstream>
#include <vulkan/vulkan.h>

#include "MemoryManager.hpp"
#include "graphics/GPU.hpp"
#include "graphics/ShaderInformation.hpp"
#include "graphics/Texture.hpp"

namespace flex {

struct Shader {
private:
  MemoryManager &mMemoryManager;

  [[nodiscard]] static VkShaderModule createShaderModule(GPU const &gpu,
                                                         std::filesystem::path const &path);

public:
  VkPipeline pipeline{};
  VkPipelineLayout pipelineLayout{};

  VkDescriptorPool descriptorPool{};
  VkDescriptorSet descriptorSet{};

  Texture texture{};

  uint32_t shaderId;

  Shader() = delete;
  Shader(GPU const &gpu, VkRenderPass const &renderPass, ShaderInformation &shaderInfo,
             MemoryManager &memoryManager);

  void destroy(GPU const &gpu);
};
} // namespace flex