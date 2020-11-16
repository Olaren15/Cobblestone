#pragma once

#include <vector>

#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/vulkan/VulkanShader.hpp"

namespace flex {
struct Scene {
  Camera camera{};
  std::vector<Mesh> meshes{};
  std::vector<VulkanShader> shaders{};

  void update();
};
} // namespace flex