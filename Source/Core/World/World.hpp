#pragma once

#include <map>
#include <vector>

#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Materials/BaseMaterial.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Shaders/BaseShader.hpp"

namespace cbl {
namespace gfx {
struct Engine;
}

struct World {
private:
  void update();

  friend struct gfx::Engine;

public:
  gfx::Camera camera;
  std::vector<gfx::Mesh> meshes;
  std::vector<gfx::BaseShader *> shaders;
  std::vector<gfx::BaseMaterial *> materials;
};
} // namespace cbl