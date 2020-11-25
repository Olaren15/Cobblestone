#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "Math/Vector/Vector2/Vector2.hpp"

namespace cbl::gfx {
struct Window {
private:
  SDL_Window *mSDLWindow{};
  bool mIsOpen = true;

public:
  Window(Window const &) = delete;
  explicit Window(int const &width = 1280, int const &height = 720, bool const &fullscreen = false);
  ~Window();

  void operator=(Window const &) = delete;
  void operator=(Window) = delete;

  void initImgui();

  void update();

  [[nodiscard]] bool isOpen() const;

  [[nodiscard]] std::vector<char const *> getRequiredVulkanExtensions() const;
  [[nodiscard]] VkSurfaceKHR getDrawableVulkanSurface(VkInstance const &vulkanInstance) const;
  [[nodiscard]] VkExtent2D getDrawableVulkanSurfaceSize() const;
};
} // namespace cbl::gfx
