#include "Input.hpp"

#include <algorithm>
#include <stdexcept>

namespace cbl {
std::vector<SDL_Event> Input::mEvents{};

void Input::updateEvents(std::vector<SDL_Event> const &events) { mEvents = events; }

bool Input::keyPressed(std::string const &keyName) {
  Uint8 const *keyStates = SDL_GetKeyboardState(nullptr);
  const SDL_Scancode scanCode = SDL_GetScancodeFromName(keyName.c_str());

  if (scanCode == SDL_SCANCODE_UNKNOWN) {
    throw std::runtime_error("Failed to parse key code");
  }

  return keyStates[scanCode];
}

Vector2<int> Input::getMouseMovement() {
  for (SDL_Event event : mEvents) {
    if (event.type == SDL_MOUSEMOTION) {
      return Vector2<int>{event.motion.xrel, event.motion.yrel};
    }
  }

  return Vector2<int>{0, 0};
}

bool Input::mouseLeftClicked() {
  return std::any_of(mEvents.begin(), mEvents.end(), [](SDL_Event e) {
    return e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT;
  });
}

bool Input::mouseRightClicked() {
  return std::any_of(mEvents.begin(), mEvents.end(), [](SDL_Event e) {
    return e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT;
  });
}

void Input::grabCursor() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_CaptureMouse(SDL_TRUE);
}

void Input::releaseCursor() {
  SDL_SetRelativeMouseMode(SDL_FALSE);
  SDL_CaptureMouse(SDL_FALSE);
}

} // namespace cbl
