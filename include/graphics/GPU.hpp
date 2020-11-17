#pragma once

#include "vulkan/vulkan.h"

#include "QueueFamilyIndices.hpp"
#include "graphics/RenderWindow.hpp"

namespace flex {
struct VulkanQueues;

struct GPU {
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

  QueueFamilyIndices queueFamilyIndices;
  VkQueue graphicsQueue{};
  VkQueue transferQueue{};
  VkQueue presentQueue{};

  void initialise(RenderWindow const &renderWindow);
  void waitIdle() const;
  void destroy() const;
};
} // namespace flex