#pragma once

#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>

#include "graphics/RenderAPI.hpp"

namespace flex {
struct RenderWindow {
private:
  unsigned int mWidth = 800;
  unsigned int mHeight = 600;
  bool mFullScreen = false;
  std::string mTitle = "Flex Engine";

  RenderAPI mRenderAPI = RenderAPI::Vulkan;

  SDL_Window *mSDLWindow{};
  bool mShouldExit = false;
  bool mHasFocus = true;

  static void initSDL();
  void createSDLWindow();

public:
  RenderWindow();
  RenderWindow(RenderWindow const &) = delete;
  RenderWindow(std::string const &title, unsigned int const &width, unsigned int const &height,
               bool const &fullscreen);
  ~RenderWindow();

  void operator=(RenderWindow const &) = delete;
  void operator=(RenderWindow) = delete;

  void update();

  [[nodiscard]] bool shouldExit() const;
  [[nodiscard]] std::string getTitle() const;
  [[nodiscard]] RenderAPI getRenderAPI() const;
  [[nodiscard]] bool hasFocus() const;
  [[nodiscard]] std::vector<char const *> getRequiredVulkanExtensions() const;
  [[nodiscard]] VkSurfaceKHR getDrawableVulkanSurface(VkInstance const &vulkanInstance) const;
  [[nodiscard]] VkExtent2D getDrawableVulkanSurfaceSize() const;
};
} // namespace flex
