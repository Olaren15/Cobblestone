﻿#include "graphics/window.hpp"

#include <stdexcept>

#include <SDL2/SDL_vulkan.h>

namespace flex {
Window::Window() {
  initSDL();
  createSDLWindow();
}

Window::Window(std::string const &title, unsigned int const &width, unsigned int const &height,
               bool const &fullscreen) {
  initSDL();

  mWidth = width;
  mHeight = height;
  mFullScreen = fullscreen;
  mTitle = title;
  createSDLWindow();
}

Window::~Window() { SDL_Quit(); }

void Window::initSDL() {
  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error(std::string("Failed to initialize SDL ") + SDL_GetError());
  }
}

void Window::createSDLWindow() {
  uint32_t windowFlags = SDL_WINDOW_SHOWN;

  if (mFullScreen) {
    windowFlags |= SDL_WINDOW_FULLSCREEN;
  }

  switch (mRenderAPI) {
  case RenderAPI::OpenGL:
    windowFlags |= SDL_WINDOW_OPENGL;
    break;
  case RenderAPI::Vulkan:
    windowFlags |= SDL_WINDOW_VULKAN;
    break;
  case RenderAPI::DirectX11:
    break;
  }

  mSDLWindow = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                static_cast<int>(mWidth), static_cast<int>(mHeight), windowFlags);

  if (mSDLWindow == nullptr) {
    throw std::runtime_error("Failed to create SDL window ");
  }
}

void Window::update() {
  SDL_Event event{};

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      mShouldExit = true;
    }
  }
}

bool Window::shouldExit() const { return mShouldExit; }

std::string Window::getTitle() const { return mTitle; }

RenderAPI Window::getRenderAPI() const { return mRenderAPI; }

std::vector<char const *> Window::getRequiredVulkanExtensions() const {
  if (mRenderAPI != RenderAPI::Vulkan)
    throw InvalidRenderAPIException{
        "Cannot get vulkan extensions if render API is not set to vulkan"};

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

vk::SurfaceKHR Window::getDrawableVulkanSurface(vk::Instance const &vulkanInstance) const {
  if (mRenderAPI != RenderAPI::Vulkan)
    throw InvalidRenderAPIException{"Cannot get drawable vulkan surface if "
                                    "render API is not set to vulkan"};

  VkSurfaceKHR surface;

  if (SDL_Vulkan_CreateSurface(mSDLWindow, {vulkanInstance}, &surface) != SDL_TRUE) {
    throw std::runtime_error("Failed to create vulkan surface");
  }

  return vk::SurfaceKHR{surface};
}

vk::Extent2D Window::getDrawableVulkanSurfaceSize() const {
  int width = 0;
  int height = 0;
  SDL_Vulkan_GetDrawableSize(mSDLWindow, &width, &height);

  return vk::Extent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

} // namespace flex
