#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "Graphics/Window/Window.hpp"
#include "Math/Vector/Vector2/Vector2.hpp"

namespace cbl {
struct Input {
private:
  static std::vector<SDL_Event> mEvents;

  Input() = default;

  static void updateEvents(std::vector<SDL_Event> const &events);
  friend struct gfx::Window;

public:
  Input(Input const &) = delete;
  void operator=(Input const &) = delete;

  [[nodiscard]] static bool keyPressed(std::string const &keyName);
  [[nodiscard]] static Vector2<int> getMouseMovement();
  [[nodiscard]] static bool mouseLeftClicked();
  [[nodiscard]] static bool mouseRightClicked();
  static void grabCursor();
  static void releaseCursor();
};
} // namespace cbl
