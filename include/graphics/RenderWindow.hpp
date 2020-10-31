#pragma once

#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "core/Vector2.hpp"
#include "graphics/RenderAPI.hpp"

namespace flex {
struct RenderWindow {
private:
  std::string mTitle = "Flex Engine";
  RenderAPI mRenderAPI = RenderAPI::Vulkan;

  SDL_Window *mSDLWindow{};
  SDL_GLContext mGLContext{};

  bool mShouldExit = false;
  bool mHasFocus = true;

  static void initSDL();
  void createSDLWindow(int const &width, int const &height, bool const &fullScreen);

public:
  RenderWindow();
  RenderWindow(RenderWindow const &) = delete;
  RenderWindow(std::string title, int const &width, int const &height, bool const &fullscreen,
               RenderAPI renderApi);
  ~RenderWindow();

  void operator=(RenderWindow const &) = delete;
  void operator=(RenderWindow) = delete;

  void update();

  [[nodiscard]] bool shouldExit() const;
  [[nodiscard]] std::string getTitle() const;
  [[nodiscard]] RenderAPI getRenderAPI() const;

  [[nodiscard]] std::vector<char const *> getRequiredVulkanExtensions() const;
  [[nodiscard]] VkSurfaceKHR getDrawableVulkanSurface(VkInstance const &vulkanInstance) const;
  [[nodiscard]] VkExtent2D getDrawableVulkanSurfaceSize() const;
};
} // namespace flex
