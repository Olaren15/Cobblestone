#pragma once

#include <filesystem>
#include <fstream>
#include <string>

#include <vulkan/vulkan.h>

#include "Graphics/GPU/GPU.hpp"
#include "Graphics/Memory/MemoryManager/MemoryManager.hpp"
#include "Graphics/Memory/Texture/Texture.hpp"

namespace cbl::gfx {

struct BaseShader {
private:
  [[nodiscard]] VkShaderModule createShaderModule(std::filesystem::path const &path) const;

protected:
  GPU const &mGPU;

  void createDefaultPipelineLayout();
  void createDefaultPipeline(VkRenderPass const &renderPass);

public:
  VkPipeline pipeline{};
  VkPipelineLayout pipelineLayout{};

  VkDescriptorSetLayout descriptorSetLayout{};
  VkDescriptorPool descriptorPool{};

  BaseShader() = delete;
  BaseShader(GPU const &gpu, VkRenderPass const &renderPass);
  virtual ~BaseShader();

  [[nodiscard]] virtual std::string getName() = 0;
};
} // namespace cbl::gfx