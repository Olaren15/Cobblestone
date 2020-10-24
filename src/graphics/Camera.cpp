#include "graphics/Camera.hpp"

#include <algorithm>

#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "core/InputHandler.hpp"
#include "core/Time.hpp"

namespace flex {
Camera::Camera() { updateVectors(); }

Camera::Camera(glm::vec3 const &position, glm::vec3 const &up, float const &yaw, float const &pitch)
    : mPosition(position), mUp(up), mYaw(yaw), mPitch(pitch) {
  updateVectors();
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

glm::mat4 Camera::getViewMatrix(float const aspectRatio) const {
  glm::mat4 const view = lookAt(mPosition, mPosition + mFront, mUp);

  glm::mat4 projection = glm::perspective(glm::radians(mFov), aspectRatio, mNearClip, mFarClip);
  projection[1][1] *= -1;

  return projection * view;
}

void Camera::update() {
  float x = 0, y = 0;
  // calculate the mouse offset since last frame
  mYaw += x * mMouseSensitivity;
  mPitch += y * mMouseSensitivity;
  mPitch = std::clamp(mPitch, -89.0f, 89.0f);

  float velocity = Time::deltaSeconds() * mMovementSpeed;
  if (InputHandler::keyPressed("Left Shift"))
    velocity *= 3.0f;
  if (InputHandler::keyPressed("w"))
    mPosition += mFront * velocity;
  if (InputHandler::keyPressed("s"))
    mPosition -= mFront * velocity;
  if (InputHandler::keyPressed("a"))
    mPosition -= mRight * velocity;
  if (InputHandler::keyPressed("d"))
    mPosition += mRight * velocity;
  if (InputHandler::keyPressed("q"))
    mPosition -= mUp * velocity;
  if (InputHandler::keyPressed("e"))
    mPosition += mUp * velocity;

  updateVectors();
}
} // namespace flex
