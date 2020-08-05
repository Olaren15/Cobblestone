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
  const bool mEnableValidationLayers = false;
#else
  const bool mEnableValidationLayers = true;
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