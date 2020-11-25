#include "Window.hpp"

#include <stdexcept>

#include <SDL2/SDL_vulkan.h>

#include "External/imgui/backends/imgui_impl_sdl.h"

#include "Core/Input/Input.hpp"

namespace cbl::gfx {
Window::Window(int const &width, int const &height, bool const &fullscreen) {
  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error(std::string("Failed to initialize SDL ") + SDL_GetError());
  }

  uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN;

  if (fullscreen) {
    windowFlags |= SDL_WINDOW_FULLSCREEN;
  }

  mSDLWindow = SDL_CreateWindow("Cobblestone", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                width, height, windowFlags);

  if (mSDLWindow == nullptr) {
    throw std::runtime_error("Failed to create SDL window");
  }
}

Window::~Window() {
  SDL_DestroyWindow(mSDLWindow);
  SDL_Quit();
}

void Window::initImgui() { ImGui_ImplSDL2_InitForVulkan(mSDLWindow); }

void Window::update() {
  SDL_Event event{};
  std::vector<SDL_Event> events;
  while (SDL_PollEvent(&event)) {
    events.push_back(event);
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      mIsOpen = false;
      break;
    default:
      break;
    }
  }

  ImGui_ImplSDL2_NewFrame(mSDLWindow);

  Input::updateEvents(events);
}

bool Window::isOpen() const { return mIsOpen; }

std::vector<char const *> Window::getRequiredVulkanExtensions() const {
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

VkSurfaceKHR Window::getDrawableVulkanSurface(VkInstance const &vulkanInstance) const {
  VkSurfaceKHR surface{};

  if (SDL_Vulkan_CreateSurface(mSDLWindow, vulkanInstance, &surface) != SDL_TRUE) {
    throw std::runtime_error("Failed to create vulkan surface");
  }

  return surface;
}

VkExtent2D Window::getDrawableVulkanSurfaceSize() const {
  int width = 0;
  int height = 0;
  SDL_Vulkan_GetDrawableSize(mSDLWindow, &width, &height);

  return VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}
} // namespace cbl::gfx
