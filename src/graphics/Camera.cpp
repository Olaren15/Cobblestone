#include "graphics/Camera.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace flex {

glm::mat4 Camera::getView(float const aspectRation) {
  glm::mat4 const view =
      lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRation, 0.1f, 10.0f);
  projection[1][1] *= -1;

  return projection * view;
}
} // namespace flex
