#pragma once

#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

namespace flex {
class InputHandler {
private:
public:
  InputHandler() = delete;
  InputHandler(InputHandler const &) = delete;
  ~InputHandler() = delete;

  void operator=(InputHandler const &) = delete;
  void operator=(InputHandler const) = delete;

  static bool keyPressed(std::string const &keyCode);
};
} // namespace flex