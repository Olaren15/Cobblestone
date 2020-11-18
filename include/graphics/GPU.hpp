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

  [[nodiscard]] static unsigned int ratePhysicalDevice(VkPhysicalDevice const &physicalDevice,
                                                VkSurfaceKHR const &vulkanSurface,
                                                std::vector<char const *> const &requiredExtensions);
  [[nodiscard]] static bool physicalDeviceSupportsExtensions(VkPhysicalDevice const &physicalDevice,
                                                      std::vector<const char *> const &extensions);

public:
  GPU() = default;
  explicit GPU(RenderWindow const &window);
  ~GPU();

  VkInstance instance{};
  VkSurfaceKHR renderSurface{};
  VkPhysicalDevice physicalDevice{};
  VkDevice device{};

  QueueFamilyIndices queueFamilyIndices;
  VkQueue graphicsQueue{};
  VkQueue transferQueue{};
  VkQueue presentQueue{};

  void waitIdle() const;

  [[nodiscard]] bool isDedicated() const;
};
} // namespace flex