#pragma once

#include <string>

namespace flex {
struct InputHandler {
  InputHandler() = delete;
  InputHandler(InputHandler const &) = delete;
  ~InputHandler() = delete;

  void operator=(InputHandler const &) = delete;

  static bool keyPressed(std::string const &keyName);
};
} // namespace flex
