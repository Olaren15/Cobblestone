#pragma once

#include <array>

#include <vulkan/vulkan.hpp>

#include "graphics/vulkan/Pipeline.hpp"
#include "graphics/vulkan/QueueFamily.hpp"
#include "graphics/vulkan/QueueFamilyIndices.hpp"
#include "graphics/vulkan/SwapChain.hpp"
#include "graphics/window.hpp"

namespace flex {
enum struct QueueFamily;

struct VulkanRenderer {
private:
#ifdef NDEBUG
  static constexpr bool mEnableValidationLayers = false;
#else
  static constexpr bool mEnableValidationLayers = true;
#endif

  static constexpr std::array<const char *, 1> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  vk::Instance mVulkanInstance;
  vk::SurfaceKHR mDrawingSurface;

  vk::PhysicalDevice mPhysicalDevice;
  QueueFamilyIndices mQueueFamilyIndices;
  vk::Device mDevice;

  SwapChain mSwapChain;
  Pipeline mPipeline;

  vk::Queue mGraphicsQueue;
  vk::Queue mPresentQueue;

  [[nodiscard]] vk::Instance createVulkanInstance(Window const &window) const;
  [[nodiscard]] vk::PhysicalDevice selectPhysicalDevice(vk::Instance const &vulkanInstance,
                                                        vk::SurfaceKHR const &vulkanSurface) const;
  [[nodiscard]] unsigned int ratePhysicalDevice(vk::PhysicalDevice const &physicalDevice,
                                                vk::SurfaceKHR const &vulkanSurface) const;
  [[nodiscard]] bool
  physicalDeviceSupportsRequiredExtensions(vk::PhysicalDevice const &physicalDevice) const;
  [[nodiscard]] vk::Device createVulkanDevice(vk::PhysicalDevice const &physicalDevice,
                                              QueueFamilyIndices const &queueFamilyIndices) const;

  [[nodiscard]] vk::Queue retrieveQueue(vk::Device const &device,
                                        QueueFamilyIndices const &queueFamilyIndices,
                                        QueueFamily const &queueFamily) const;

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(Window const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;
};
} // namespace flex
