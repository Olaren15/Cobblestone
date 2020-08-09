#include "core/inputHandler.hpp"

namespace flex {

bool InputHandler::keyPressed(std::string const &keyName) {

  Uint8 const *keyStates = SDL_GetKeyboardState(nullptr);
  SDL_Scancode scanCode = SDL_GetScancodeFromName(keyName.c_str());

  if (scanCode == SDL_Scancode::SDL_SCANCODE_UNKNOWN) {
    throw std::runtime_error("Failed to parse key code");
  }

  return keyStates[scanCode];
}

} // namespace flex