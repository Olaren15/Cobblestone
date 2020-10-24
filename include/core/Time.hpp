#pragma once

#include <cstdint>

namespace flex {
struct Time {
private:
  static uint32_t mLastTick;
  static float mDeltaSeconds;

  static void tick();

  friend struct RenderWindow;

public:
  Time() = delete;
  Time(Time &) = delete;
  Time(Time const &) = delete;
  ~Time() = delete;

  static float deltaSeconds();
};
} // namespace flex