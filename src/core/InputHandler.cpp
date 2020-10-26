#include "core/InputHandler.hpp"

#include <stdexcept>
#include <string>

namespace flex {
InputHandler &InputHandler::get() {
  static InputHandler inputHandler{};

  return inputHandler;
}

void InputHandler::updateEvents(std::vector<SDL_Event> const &events) { mEvents = events; }

bool InputHandler::keyPressed(std::string const &keyName) const {
  Uint8 const *keyStates = SDL_GetKeyboardState(nullptr);
  const SDL_Scancode scanCode = SDL_GetScancodeFromName(keyName.c_str());

  if (scanCode == SDL_SCANCODE_UNKNOWN) {
    throw std::runtime_error("Failed to parse key code");
  }

  return keyStates[scanCode];
}

Vector2<int> InputHandler::getMouseMovement() {
  for (SDL_Event event : mEvents) {
    if (event.type == SDL_MOUSEMOTION) {
      return Vector2<int>{event.motion.xrel, event.motion.yrel};
    }
  }

  return Vector2<int>{0, 0};
}

bool InputHandler::mouseLeftClicked() {
  for (SDL_Event event : mEvents) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
      return true;
    }
  }

  return false;
}

bool InputHandler::mouseRightClicked() {

  for (SDL_Event event : mEvents) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
      return true;
    }
  }

  return false;
}

void InputHandler::grabCursor() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_CaptureMouse(SDL_TRUE);
}

void InputHandler::releaseCursor() {
  SDL_SetRelativeMouseMode(SDL_FALSE);
  SDL_CaptureMouse(SDL_FALSE);
}

} // namespace flex
