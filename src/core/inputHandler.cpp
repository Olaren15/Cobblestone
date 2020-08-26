#include "core/InputHandler.hpp"

#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

namespace flex {
bool InputHandler::keyPressed(std::string const &keyName) {
  Uint8 const *keyStates = SDL_GetKeyboardState(nullptr);
  const SDL_Scancode scanCode = SDL_GetScancodeFromName(keyName.c_str());

  if (scanCode == SDL_SCANCODE_UNKNOWN) {
    throw std::runtime_error("Failed to parse key code");
  }

  return keyStates[scanCode];
}
} // namespace flex
