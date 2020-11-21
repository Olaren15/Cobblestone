#pragma once

#include <vulkan/vulkan.h>

#include "graphics/Image.hpp"

namespace flex {
struct Texture {
  Image image{};
  VkSampler sampler{};
};
} // namespace flex