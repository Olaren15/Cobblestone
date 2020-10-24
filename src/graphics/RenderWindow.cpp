#include "graphics/RenderWindow.hpp"

#include <stdexcept>

#include <SDL2/SDL_vulkan.h>

#include "core/Time.hpp"

namespace flex {
RenderWindow::RenderWindow() {
  initSDL();
  createSDLWindow();
}

RenderWindow::RenderWindow(std::string const &title, unsigned int const &width,
                           unsigned int const &height, bool const &fullscreen,
                           RenderAPI renderApi) {
  initSDL();

  mWidth = width;
  mHeight = height;
  mFullScreen = fullscreen;
  mTitle = title;
  mRenderAPI = renderApi;
  createSDLWindow();
}

RenderWindow::~RenderWindow() {
  if (mRenderAPI == RenderAPI::OpenGL && mGLContext != nullptr) {
    SDL_GL_DeleteContext(mGLContext);
  }

  SDL_Quit();
}

void RenderWindow::initSDL() {
  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error(std::string("Failed to initialize SDL ") + SDL_GetError());
  }
}

void RenderWindow::createSDLWindow() {
  uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

  if (mFullScreen) {
    windowFlags |= SDL_WINDOW_FULLSCREEN;
  }

  switch (mRenderAPI) {
  case RenderAPI::OpenGL:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    windowFlags |= SDL_WINDOW_OPENGL;
    break;
  case RenderAPI::Vulkan:
    windowFlags |= SDL_WINDOW_VULKAN;
    break;
  }

  mSDLWindow = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                static_cast<int>(mWidth), static_cast<int>(mHeight), windowFlags);

  if (mSDLWindow == nullptr) {
    throw std::runtime_error("Failed to create SDL window");
  }

  if (mRenderAPI == RenderAPI::OpenGL) {
    mGLContext = SDL_GL_CreateContext(mSDLWindow);
    if (mGLContext == nullptr) {
      throw std::runtime_error("Failed to create OpenGL context");
    }

    // disable vsync
    SDL_GL_SetSwapInterval(0);
  }
}

void RenderWindow::update() {
  if (mRenderAPI == RenderAPI::OpenGL) {
    SDL_GL_SwapWindow(mSDLWindow);
  }

  SDL_Event event{};

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      mShouldExit = true;
      break;
    case SDL_WINDOWEVENT: {
      switch (event.window.event) {
      case SDL_WINDOWEVENT_FOCUS_LOST:
        mHasFocus = false;
        break;
      case SDL_WINDOWEVENT_FOCUS_GAINED:
        mHasFocus = true;
        break;
      }
    }
    default:
      break;
    }
  }
}

bool RenderWindow::shouldExit() const { return mShouldExit; }

std::string RenderWindow::getTitle() const { return mTitle; }

RenderAPI RenderWindow::getRenderAPI() const { return mRenderAPI; }

bool RenderWindow::hasFocus() const { return mHasFocus; }

std::vector<char const *> RenderWindow::getRequiredVulkanExtensions() const {
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

VkSurfaceKHR RenderWindow::getDrawableVulkanSurface(VkInstance const &vulkanInstance) const {
  if (mRenderAPI != RenderAPI::Vulkan)
    throw InvalidRenderAPIException{"Cannot get drawable vulkan surface if "
                                    "render API is not set to vulkan"};

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
} // namespace flex
