#include "RenderWindow.hpp"

#include <stdexcept>

#include <SDL2/SDL_vulkan.h>

#include "Core/Input/InputHandler.hpp"

namespace cbl::gfx {
RenderWindow::RenderWindow() {
  initSDL();
  createSDLWindow(1280, 720, false);
}

RenderWindow::RenderWindow(int const &width, int const &height, bool const &fullscreen) {
  initSDL();
  createSDLWindow(width, height, fullscreen);
}

RenderWindow::~RenderWindow() {
  SDL_DestroyWindow(mSDLWindow);
  SDL_Quit();
}

void RenderWindow::initSDL() {
  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error(std::string("Failed to initialize SDL ") + SDL_GetError());
  }
}

void RenderWindow::createSDLWindow(int const &width, int const &height, bool const &fullScreen) {
  uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN;

  if (fullScreen) {
    windowFlags |= SDL_WINDOW_FULLSCREEN;
  }

  mSDLWindow = SDL_CreateWindow("Flex Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                width, height, windowFlags);

  if (mSDLWindow == nullptr) {
    throw std::runtime_error("Failed to create SDL window");
  }
}

void RenderWindow::update() {
  SDL_Event event{};
  std::vector<SDL_Event> events;
  while (SDL_PollEvent(&event)) {
    events.push_back(event);
    switch (event.type) {
    case SDL_QUIT:
      mIsOpen = false;
      break;
    default:
      break;
    }
  }

  Input.updateEvents(events);
}

bool RenderWindow::isOpen() const { return mIsOpen; }

std::vector<char const *> RenderWindow::getRequiredVulkanExtensions() const {
  unsigned int count = 0;

  if (!SDL_Vulkan_GetInstanceExtensions(mSDLWindow, &count, nullptr)) {
    throw std::runtime_error("Failed to get required Vulkan extensions");
  }

  std::vector<char const *> extensions(count);

  if (!SDL_Vulkan_GetInstanceExtensions(mSDLWindow, &count, extensions.data())) {
    throw std::runtime_error("Failed to get required Vulkan extensions");
  }

  return extensions;
}

VkSurfaceKHR RenderWindow::getDrawableVulkanSurface(VkInstance const &vulkanInstance) const {
  VkSurfaceKHR surface{};

  if (SDL_Vulkan_CreateSurface(mSDLWindow, vulkanInstance, &surface) != SDL_TRUE) {
    throw std::runtime_error("Failed to create vulkan surface");
  }

  return surface;
}

VkExtent2D RenderWindow::getDrawableVulkanSurfaceSize() const {
  int width = 0;
  int height = 0;
  SDL_Vulkan_GetDrawableSize(mSDLWindow, &width, &height);

  return VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}
} // namespace cbl::gfx
