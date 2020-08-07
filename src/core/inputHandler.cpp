#include "core/inputHandler.hpp"

namespace flex {

bool InputHandler::keyPressed(std::string const &keyName) {

  Uint8 const *keystates = SDL_GetKeyboardState(NULL);
  SDL_Scancode scanCode = SDL_GetScancodeFromName(keyName.c_str());

  if (scanCode == SDL_Scancode::SDL_SCANCODE_UNKNOWN) {
    throw std::runtime_error("Failed to parse key code");
  }

  return keystates[scanCode];
}

} // namespace flex