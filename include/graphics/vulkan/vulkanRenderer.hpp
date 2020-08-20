#pragma once

#include <array>

#include <vulkan/vulkan.hpp>

#include "graphics/vulkan/QueueFamily.hpp"
#include "graphics/vulkan/QueueFamilyIndices.hpp"
#include "graphics/vulkan/SwapChainSupportDetails.hpp"
#include "graphics/window.hpp"

namespace flex {
enum struct QueueFamily;

class VulkanRenderer {
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

  SwapChainSupportDetails mSwapChainSupportDetails;
  vk::SwapchainKHR mSwapChain;

  vk::Queue mGraphicsQueue;
  vk::Queue mPresentQueue;

  [[nodiscard]] static vk::Instance createVulkanInstance(Window const &window);
  [[nodiscard]] static vk::PhysicalDevice selectPhysicalDevice(vk::Instance const &vulkanInstance,
                                                               vk::SurfaceKHR const &vulkanSurface);
  [[nodiscard]] static unsigned int ratePhysicalDevice(vk::PhysicalDevice const &physicalDevice,
                                                       vk::SurfaceKHR const &vulkanSurface);
  [[nodiscard]] static bool
  physicalDeviceSupportsRequiredExtensions(vk::PhysicalDevice const &physicalDevice);
  [[nodiscard]] static vk::Device createVulkanDevice(vk::PhysicalDevice const &physicalDevice,
                                                     QueueFamilyIndices const &queueFamilyIndices);

  [[nodiscard]] static vk::Queue retrieveQueue(vk::Device const &device,
                                               QueueFamilyIndices const &queueFamilyIndices,
                                               QueueFamily const &queueFamily);

  [[nodiscard]] static vk::SurfaceFormatKHR
  chooseSwapChainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats);
  [[nodiscard]] static vk::PresentModeKHR
  chooseSwapChainPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes);
  [[nodiscard]] static vk::Extent2D
  chooseSwapChainExtent(vk::SurfaceCapabilitiesKHR const &capabilities, Window const &window);
  [[nodiscard]] static vk::SwapchainKHR
  createSwapChain(vk::Device const &device, Window const &window, vk::SurfaceKHR const &surface,
                  QueueFamilyIndices const &queueFamilyIndices,
                  SwapChainSupportDetails const &swapChainSupportDetails,
                  vk::SwapchainKHR const &oldSwapChain);

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(Window const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;
};
} // namespace flex
