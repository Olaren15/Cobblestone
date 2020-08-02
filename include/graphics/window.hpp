#pragma once

#include <stdexcept>

#include <SDL2/SDL.h>

#include "core/inputHandler.hpp"
#include "renderAPI.hpp"

namespace flex {
class Window {
private:
  unsigned int mWidth;
  unsigned int mHeight;
  bool mFullscreen;
  std::string mTitle;

  RenderAPI mRenderAPI;

  SDL_Window *mSdlWindow;
  bool mShouldExit;

  void initSDL() const;

  SDL_Window *createSDLWindow() const;

public:
  Window();
  Window(const std::string &title, const unsigned int &width,
         const unsigned int &height, const bool &fullscreen);
  ~Window();

  void update();

  bool shouldExit() const;
};
} // namespace flex