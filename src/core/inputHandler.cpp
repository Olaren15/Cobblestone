#include "core/inputHandler.hpp"

namespace flex {

bool InputHandler::keyPressed(const std::string &keyName) {

  const Uint8 *keystates = SDL_GetKeyboardState(NULL);
  SDL_Scancode scanCode = SDL_GetScancodeFromName(keyName.c_str());

  if (scanCode == SDL_Scancode::SDL_SCANCODE_UNKNOWN) {
    throw std::runtime_error("Failed to parse key code");
  }

  return keystates[scanCode];
}

} // namespace flex