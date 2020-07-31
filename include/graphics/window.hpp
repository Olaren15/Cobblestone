#pragma once

#include <stdexcept>

#include <SDL2/SDL.h>

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

  void initSDL() const;

  SDL_Window *createSdlWindow() const;

public:
  Window();
  Window(const std::string &title, const unsigned int &width,
         const unsigned int &height, const bool &fullscreen);
  ~Window();
};
} // namespace flex