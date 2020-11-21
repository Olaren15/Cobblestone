#pragma once

#include <map>
#include <vector>

#include "graphics/Camera.hpp"
#include "graphics/Material.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"

namespace flex {
struct Scene {
  Camera camera;
  std::vector<Mesh> meshes;
  std::vector<Shader> shaders;
  std::vector<Material> materials;

  void update();
};
} // namespace flex