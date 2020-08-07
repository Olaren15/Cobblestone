#pragma once

#include <iostream>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "graphics/renderAPI.hpp"
#include "graphics/window.hpp"

namespace flex {
class VulkanRenderer {
private:
#ifdef NDEBUG
  static constexpr bool mEnableValidationLayers = false;
#else
  static constexpr bool mEnableValidationLayers = true;
#endif
  vk::Instance mVulkanInstance;

  void createVulkanInstance(Window const &window);

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  VulkanRenderer(Window const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer const) = delete;
};
} // namespace flex