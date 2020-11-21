#pragma once

#include <filesystem>

#include <fstream>
#include <vulkan/vulkan.h>

#include "MemoryManager.hpp"
#include "graphics/GPU.hpp"
#include "graphics/Texture.hpp"

namespace flex {

struct Shader {
private:
  [[nodiscard]] static VkShaderModule createShaderModule(GPU const &gpu,
                                                         std::filesystem::path const &path);

public:
  VkPipeline pipeline{};
  VkPipelineLayout pipelineLayout{};

  VkDescriptorSetLayout descriptorSetLayout{};
  VkDescriptorPool descriptorPool{};

  Shader() = delete;
  Shader(GPU const &gpu, VkRenderPass const &renderPass);

  void destroy(GPU const &gpu);
};
} // namespace flex