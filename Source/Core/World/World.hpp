#pragma once

#include <map>
#include <vector>

#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Materials/BaseMaterial.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Shaders/BaseShader.hpp"

namespace cbl {
struct World {
  gfx::Camera camera;
  std::vector<gfx::Mesh> meshes;
  std::vector<gfx::BaseShader *> shaders;
  std::vector<gfx::BaseMaterial *> materials;

  void update();
};
} // namespace flex