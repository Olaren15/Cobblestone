#include "graphics/window.hpp"

namespace flex {
Window::Window()
    : mWidth(800), mHeight(600), mFullscreen(false), mTitle("Flex Engine"),
      mRenderAPI(RenderAPI::OpenGL) {

  initSDL();
  mSdlWindow = createSDLWindow();
}

Window::Window(const std::string &title, const unsigned int &width,
               const unsigned int &height, const bool &fullscreen)
    : mWidth(width), mHeight(height), mFullscreen(fullscreen), mTitle(title),
      mRenderAPI(RenderAPI::OpenGL) {

  initSDL();
  mSdlWindow = createSDLWindow();
}

Window::~Window() { SDL_Quit(); }

void Window::initSDL() const {
  int success = SDL_Init(SDL_INIT_EVERYTHING);

  if (success != 0) {
    throw std::runtime_error(std::string("Failed to initialize SDL ") +
                             SDL_GetError());
  }
}

SDL_Window *Window::createSDLWindow() const {
  SDL_Window *window;

  uint32_t windowFlags = SDL_WindowFlags::SDL_WINDOW_SHOWN;

  if (mFullscreen) {
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

  window =
      SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, windowFlags);

  if (window == NULL) {
    throw std::runtime_error(std::string("Failed to create SDL window ") +
                             SDL_GetError());
  }

  return window;
}

void Window::update() {
  SDL_Event event;

  EventHandler::get().clearEvents();

  while (SDL_PollEvent(&event)) {
    EventHandler::get().consumeEvent(event);
  }
}
} // namespace flex
