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
  unsigned int mWidth = 800;
  unsigned int mHeight = 600;
  bool mFullScreen = false;
  std::string mTitle = "Flex Engine";

  RenderAPI mRenderAPI = RenderAPI::Vulkan;

  SDL_Window *mSDLWindow;
  bool mShouldExit = false;

  void initSDL() const;

  SDL_Window *createSDLWindow() const;

public:
  Window();
  Window(Window const &) = delete;
  Window(std::string const &title, unsigned int const &width, unsigned int const &height, bool const &fullscreen);
  ~Window();

  void operator=(Window const &) = delete;
  void operator=(Window const) = delete;

  void update();

  bool shouldExit() const;
  std::string getTitle() const;
  RenderAPI getRenderAPI() const;
  std::vector<char const *> getRequiredVulkanExtensions() const;
};
} // namespace flex