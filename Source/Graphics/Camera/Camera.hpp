#pragma once

#include <glm/glm.hpp>

#include "Graphics/Render/Window/RenderWindow.hpp"
#include "Math/Vector/Vector2/Vector2.hpp"

namespace cbl::gfx {
struct Camera {
private:
  glm::vec3 mPosition = glm::vec3(0.0f, 0.0f, 2.0f);
  glm::vec3 mFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 mUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 mRight = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

  float mYaw = -90.0f;
  float mPitch = 0.0f;

  float mFov = 90.0f;
  float mNearClip = 0.01f;
  float mFarClip = 50.0f;

  float mMovementSpeed = 2.25f;
  float mMouseSensitivity = 0.1f;

  bool mControlsEnabled = true;

  void handleMouse();
  void handleKeyboard();
  void updateVectors();

public:
  Camera();
  Camera(glm::vec3 const &position, glm::vec3 const &up, float const &yaw, float const &pitch);

  void update();

  [[nodiscard]] glm::mat4 getViewMatrix(float aspectRatio) const;
};

} // namespace flex