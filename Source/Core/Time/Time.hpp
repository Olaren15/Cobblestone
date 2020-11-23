#pragma once

#include <cstdint>

#include "Graphics/Engine/Engine.hpp"

namespace cbl {
struct Time {
private:
  static uint32_t mLastTick;
  static float mDeltaSeconds;

  static void tick();
  friend struct gfx::Engine;

public:
  Time() = delete;
  Time(Time &) = delete;
  Time(Time const &) = delete;
  ~Time() = delete;

  static float deltaSeconds();
};
} // namespace cbl