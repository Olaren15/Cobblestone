#pragma once

#include <vulkan/vulkan.h>

#include "Graphics/Memory/Image/Image.hpp"

namespace cbl::gfx::mem {
struct Texture {
  Image image{};
  VkSampler sampler{};
};
} // namespace cbl::gfx::mem