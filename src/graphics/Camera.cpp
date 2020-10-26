#include "graphics/Camera.hpp"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "core/InputHandler.hpp"
#include "core/Time.hpp"

namespace flex {
Camera::Camera() {
  Input.grabCursor();
  updateVectors();
}

Camera::Camera(glm::vec3 const &position, glm::vec3 const &up, float const &yaw, float const &pitch)
    : mPosition(position), mUp(up), mYaw(yaw), mPitch(pitch) {
  Input.grabCursor();
  updateVectors();
}

void Camera::handleMouse() {
  Vector2<int> currentMousePosition = Input.getMouseMovement();
  mYaw += currentMousePosition.x * mMouseSensitivity;
  mPitch += currentMousePosition.y * mMouseSensitivity;
  mPitch = std::clamp(mPitch, -89.0f, 89.0f);
}

void Camera::handleKeyboard() {
  float velocity = Time::deltaSeconds() * mMovementSpeed;
  if (Input.keyPressed("Left Shift"))
    velocity *= 3.0f;
  if (Input.keyPressed("w"))
    mPosition += mFront * velocity;
  if (Input.keyPressed("s"))
    mPosition -= mFront * velocity;
  if (Input.keyPressed("a"))
    mPosition -= mRight * velocity;
  if (Input.keyPressed("d"))
    mPosition += mRight * velocity;
  if (Input.keyPressed("q"))
    mPosition += mUp * velocity;
  if (Input.keyPressed("e"))
    mPosition -= mUp * velocity;
}

void Camera::updateVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
  front.y = sin(glm::radians(mPitch));
  front.z = sin(glm::radians(mYaw));
  mFront = glm::normalize(front);
  mRight = glm::normalize(glm::cross(mFront, mWorldUp));
  mUp = glm::normalize(glm::cross(mRight, mFront));
}

void Camera::update() {
  if (mControlsEnabled) {
    if (Input.keyPressed("Escape")) {
      Input.releaseCursor();
      mControlsEnabled = false;
    }
    handleMouse();
    handleKeyboard();
  } else {
    if (Input.mouseLeftClicked()) {
      Input.grabCursor();
      mControlsEnabled = true;
    }
  }

  updateVectors();
}

glm::mat4 Camera::getViewMatrix(float const aspectRatio) const {
  glm::mat4 const view = lookAt(mPosition, mPosition + mFront, mUp);
  glm::mat4 const projection =
      glm::perspective(glm::radians(mFov), aspectRatio, mNearClip, mFarClip);

  return projection * view;
}
} // namespace flex
