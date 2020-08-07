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

  void createVulkanInstance(const Window &window);

public:
  VulkanRenderer() = delete;
  VulkanRenderer(const VulkanRenderer &) = delete;
  VulkanRenderer(const Window &window);
  ~VulkanRenderer();

  void operator=(const VulkanRenderer &) = delete;
  void operator=(const VulkanRenderer) = delete;
};
} // namespace flex