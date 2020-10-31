#pragma once

#include "vulkan/vulkan.h"

#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/VulkanQueueFamilyIndices.hpp"

namespace flex {
struct VulkanQueues;

struct VulkanGPU {
private:
  std::vector<const char *> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  void createInstance(RenderWindow const &renderWindow);
  void selectPhysicalDevice();
  void createDevice();
  void retrieveQueues();

public:
  VkInstance instance{};
  VkSurfaceKHR renderSurface{};
  VkPhysicalDevice physicalDevice{};
  VkDevice device{};

  VulkanQueueFamilyIndices queueFamilyIndices;
  VkQueue graphicsQueue{};
  VkQueue transferQueue{};
  VkQueue presentQueue{};

  void initialise(RenderWindow const &renderWindow);
  void waitIdle() const;
  void destroy() const;
};
} // namespace flex