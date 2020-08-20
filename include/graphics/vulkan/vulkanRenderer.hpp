#pragma once

#include <vulkan/vulkan.hpp>

#include "graphics/window.hpp"
#include "graphics/vulkan/QueueFamilyIndices.hpp"
#include "graphics/vulkan/VulkanQueueFamily.hpp"

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
  QueueFamilyIndices mQueueFamilyIndices;
  vk::Device mDevice;

  vk::Queue mGraphicsQueue;

  [[nodiscard]] static vk::Instance createVulkanInstance(Window const &window);
  [[nodiscard]] static vk::PhysicalDevice selectPhysicalDevice(
      vk::Instance const &vulkanInstance);
  [[nodiscard]] static unsigned int ratePhysicalDevice(
      vk::PhysicalDevice const &physicalDevice);
  [[nodiscard]] static vk::Device createVulkanDevice(
      vk::PhysicalDevice const &physicalDevice,
      QueueFamilyIndices const &queueFamilyIndices);

  [[nodiscard]] static vk::Queue retrieveQueue(vk::Device const &device,
                                               QueueFamilyIndices
                                               const &
                                               queueFamilyIndices,
                                               QueueFamily const &queueFamily);

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(Window const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;
};
} // namespace flex
