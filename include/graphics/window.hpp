#pragma once

#include <stdexcept>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

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

  SDL_Window *mSDLWindow;
  bool mShouldExit;

  void initSDL() const;

  SDL_Window *createSDLWindow() const;

public:
  Window();
  Window(const Window &) = delete;
  Window(const std::string &title, const unsigned int &width,
         const unsigned int &height, const bool &fullscreen);
  ~Window();

  void operator=(const Window &) = delete;
  void operator=(const Window) = delete;

  void update();

  bool shouldExit() const;
  std::string getTitle() const;
  RenderAPI getRenderAPI() const;
  std::vector<const char *> getRequiredVulkanExtensions() const;
};
} // namespace flex