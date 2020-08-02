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
  void operator=(InputHandler) = delete;

  static InputHandler &get();

  static bool keyPressed(const std::string &keyCode);
};
} // namespace flex