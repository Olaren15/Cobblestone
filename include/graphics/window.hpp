#pragma once

#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>

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

  static void initSDL();

  [[nodiscard]] SDL_Window *createSDLWindow() const;

public:
  Window();
  Window(Window const &) = delete;
  Window(std::string const &title, unsigned int const &width,
         unsigned int const &height, bool const &fullscreen);
  ~Window();

  void operator=(Window const &) = delete;
  void operator=(Window) = delete;

  void update();

  [[nodiscard]] bool shouldExit() const;
  [[nodiscard]] std::string getTitle() const;
  [[nodiscard]] RenderAPI getRenderAPI() const;
  [[nodiscard]] std::vector<char const *> getRequiredVulkanExtensions() const;
  [[nodiscard]] vk::SurfaceKHR getDrawableVulkanSurface(
      vk::Instance const &vulkanInstance) const;
};
} // namespace flex
