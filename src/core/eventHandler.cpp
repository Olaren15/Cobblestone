#include "core/eventHandler.hpp"

namespace flex {
EventHandler::EventHandler() {}

EventHandler::~EventHandler() {}

EventHandler &EventHandler::get() {
  static EventHandler instance;
  return instance;
}

void EventHandler::clearEvents() { mCurrentFrameEvents.clear(); }

void EventHandler::consumeEvent(SDL_Event eventToConsume) {
  mCurrentFrameEvents.push_back(eventToConsume);
}

bool EventHandler::keyPressed(const std::string &keyName) const {

  const Uint8 *keystates = SDL_GetKeyboardState(NULL);
  SDL_Scancode scanCode = SDL_GetScancodeFromName(keyName.c_str());

  if (scanCode == SDL_Scancode::SDL_SCANCODE_UNKNOWN) {
    throw std::runtime_error("Failed to parse key code");
  }

  return keystates[scanCode];
}

bool EventHandler::keyDown(const std::string &keyCode) const {
  for (const SDL_Event &event : mCurrentFrameEvents) {
    if (event.type == SDL_EventType::SDL_KEYDOWN) {
      if (SDL_GetKeyName(event.key.keysym.sym) == keyCode.c_str()) {
        return true;
      }
    }
  }

  return false;
}

bool EventHandler::keyUp(const std::string &keyCode) const {
  for (const SDL_Event &event : mCurrentFrameEvents) {
    if (event.type == SDL_EventType::SDL_KEYUP) {
      if (SDL_GetKeyName(event.key.keysym.sym) == keyCode.c_str()) {
        return true;
      }
    }
  }

  return false;
}

bool EventHandler::shouldExit() {
  for (const SDL_Event &event : mCurrentFrameEvents) {
    if (event.type == SDL_EventType::SDL_QUIT) {
      return true;
    }
  }

  return false;
}

} // namespace flex