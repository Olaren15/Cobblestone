#pragma once

#include <vulkan/vulkan.hpp>

#include "graphics/window.hpp"
#include "graphics/vulkan/vulkanDeviceQueueFamilyIndices.hpp"

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

  vk::Instance createVulkanInstance(Window const &window) const;
  static vk::PhysicalDevice selectPhysicalDevice(vk::Instance const &vulkanInstance);
  static unsigned int ratePhysicalDevice(vk::PhysicalDevice const &physicalDevice);

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(Window const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;
};
} // namespace flex
