#pragma once

#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

namespace flex {
class InputHandler {
private:
public:
  InputHandler() = delete;
  InputHandler(const InputHandler &) = delete;
  ~InputHandler() = delete;

  void operator=(const InputHandler &) = delete;
  void operator=(const InputHandler) = delete;

  static bool keyPressed(const std::string &keyCode);
};
} // namespace flex