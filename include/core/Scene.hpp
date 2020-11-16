#pragma once

#include <vector>

#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"

namespace flex {
struct Scene {
  Camera camera{};
  std::vector<Mesh> meshes{};
  std::vector<Shader> shaders{};

  void update();
};
} // namespace flex