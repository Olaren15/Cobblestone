#pragma once

#include <iostream>
#include <map>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "graphics/renderAPI.hpp"
#include "graphics/vulkan/vulkanDeviceQueueFamilyIndices.hpp"
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
  vk::PhysicalDevice mPhysicalDevice;
  VulkanDeviceQueueFamilyIndices mQueueFamilyIndices;

  vk::Instance createVulkanInstance(Window const &window);
  vk::PhysicalDevice selectPhysicalDevice(vk::Instance const &vulkanInstance);
  unsigned int ratePhysicalDevice(vk::PhysicalDevice const &physicalDevice);

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  VulkanRenderer(Window const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer const) = delete;
};
} // namespace flex