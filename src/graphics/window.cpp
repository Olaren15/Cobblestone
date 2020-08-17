#include "graphics/window.hpp"

namespace flex {
Window::Window() {
  initSDL();
  mSDLWindow = createSDLWindow();
}

Window::Window(std::string const &title, unsigned int const &width, unsigned int const &height,
               bool const &fullscreen) {
  initSDL();

  mWidth = width;
  mHeight = height;
  mFullScreen = fullscreen;
  mTitle = title;
  mSDLWindow = createSDLWindow();
}

Window::~Window() { SDL_Quit(); }

void Window::initSDL() const {
  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error(std::string("Failed to initialize SDL ") + SDL_GetError());
  }
}

SDL_Window *Window::createSDLWindow() const {
  uint32_t windowFlags = SDL_WindowFlags::SDL_WINDOW_SHOWN;

  if (mFullScreen) {
    windowFlags |= SDL_WindowFlags::SDL_WINDOW_FULLSCREEN;
  }

  switch (mRenderAPI) {
  case RenderAPI::OpenGL:
    windowFlags |= SDL_WindowFlags::SDL_WINDOW_OPENGL;
    break;
  case RenderAPI::Vulkan:
    windowFlags |= SDL_WindowFlags::SDL_WINDOW_VULKAN;
    break;
  default:
    break;
  }

  SDL_Window *window =
      SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, windowFlags);

  if (window == nullptr) {
    throw std::runtime_error("Failed to create SDL window ");
  }

  return window;
}

void Window::update() {
  SDL_Event event{};

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EventType::SDL_QUIT) {
      mShouldExit = true;
    }
  }
}

bool Window::shouldExit() const { return mShouldExit; }
std::string Window::getTitle() const { return mTitle; }
RenderAPI Window::getRenderAPI() const { return mRenderAPI; }
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
} // namespace flex