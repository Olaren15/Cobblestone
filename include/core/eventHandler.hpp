#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

namespace flex {
class EventHandler {
private:
  std::vector<SDL_Event> mCurrentFrameEvents;

  EventHandler();
  ~EventHandler();

public:
  EventHandler(EventHandler const &) = delete;
  void operator=(EventHandler const &) = delete;

  static EventHandler &get();

  void clearEvents();
  void consumeEvent(SDL_Event eventToComsume);

  bool keyPressed(const std::string &keyCode) const;
  bool keyDown(const std::string &keyCode) const;
  bool keyUp(const std::string &keyCode) const;

  bool shouldExit();
};
} // namespace flex