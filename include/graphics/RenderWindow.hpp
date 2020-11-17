#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "core/Vector2.hpp"

namespace flex {
struct RenderWindow {
private:
  SDL_Window *mSDLWindow{};
  bool mIsOpen = true;

  static void initSDL();
  void createSDLWindow(int const &width, int const &height, bool const &fullScreen);

public:
  RenderWindow();
  RenderWindow(RenderWindow const &) = delete;
  RenderWindow(int const &width, int const &height, bool const &fullscreen);
  ~RenderWindow();

  void operator=(RenderWindow const &) = delete;
  void operator=(RenderWindow) = delete;

  void update();

  [[nodiscard]] bool isOpen() const;

  [[nodiscard]] std::vector<char const *> getRequiredVulkanExtensions() const;
  [[nodiscard]] VkSurfaceKHR getDrawableVulkanSurface(VkInstance const &vulkanInstance) const;
  [[nodiscard]] VkExtent2D getDrawableVulkanSurfaceSize() const;
};
} // namespace flex
