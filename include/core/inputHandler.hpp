#pragma once

#include <string>

namespace flex {
class InputHandler {
private:
public:
  InputHandler() = delete;
  InputHandler(InputHandler const &) = delete;
  ~InputHandler() = delete;

  void operator=(InputHandler const &) = delete;

  static bool keyPressed(std::string const &keyName);
};
} // namespace flex
