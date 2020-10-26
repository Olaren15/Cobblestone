#pragma once

#include <string>
#include <vector>

#include <Sdl2/SDL.h>

#include <core/Vector2.hpp>

#define Input InputHandler::get()

namespace flex {
struct InputHandler {
private:
  std::vector<SDL_Event> mEvents;

  InputHandler() = default;

public:
  InputHandler(InputHandler const &) = delete;
  void operator=(InputHandler const &) = delete;

  static InputHandler &get();
  void updateEvents(std::vector<SDL_Event> const &events);

  [[nodiscard]] bool keyPressed(std::string const &keyName) const;
  [[nodiscard]] Vector2<int> getMouseMovement();
  [[nodiscard]] bool mouseLeftClicked();
  [[nodiscard]] bool mouseRightClicked();
  void grabCursor();
  void releaseCursor();
};
} // namespace flex
