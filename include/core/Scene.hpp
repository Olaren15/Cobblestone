#pragma once

#include <vector>

#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"

namespace flex {
struct Scene {
  Camera camera{};
  std::vector<Mesh> meshes{};

  void update();
};
} // namespace flex