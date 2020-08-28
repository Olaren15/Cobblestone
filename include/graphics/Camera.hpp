#pragma once

#include <glm/glm.hpp>

namespace flex {
struct Camera {
  [[nodiscard]] static glm::mat4 getView(float aspectRation);
};

} // namespace flex