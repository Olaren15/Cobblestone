#pragma once

#include <optional>
#include <set>

#include "vulkan/vulkan.h"

#include "Graphics/Window/Window.hpp"

namespace cbl::gfx {
struct QueueFamilyIndices {
  uint32_t graphics{};
  uint32_t transfer{};
  uint32_t present{};

  QueueFamilyIndices() = default;
  QueueFamilyIndices(QueueFamilyIndices const &queueFamilyIndices);
  explicit QueueFamilyIndices(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface);
  ~QueueFamilyIndices() = default;

  [[nodiscard]] std::set<uint32_t> getUniqueIndices() const;
};

struct GPU {
private:
  std::vector<const char *> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  void createInstance(Window const &renderWindow);
  void selectPhysicalDevice();
  void createDevice();
  void retrieveQueues();

  [[nodiscard]] static unsigned int
  ratePhysicalDevice(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &vulkanSurface,
                     std::vector<char const *> const &requiredExtensions);
  [[nodiscard]] static bool
  physicalDeviceSupportsExtensions(VkPhysicalDevice const &physicalDevice,
                                   std::vector<const char *> const &extensions);

public:
  GPU() = default;
  explicit GPU(Window const &window);
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
} // namespace cbl::gfx