#include "graphics/window.hpp"

namespace flex {
Window::Window() : mWidth(800), mHeight(600), mFullscreen(false) {}

Window::Window(const unsigned int &width, const unsigned int &height,
               const bool &fullscreen)
    : mWidth(width), mHeight(height), mFullscreen(fullscreen) {}

Window::~Window() {}
} // namespace flex
