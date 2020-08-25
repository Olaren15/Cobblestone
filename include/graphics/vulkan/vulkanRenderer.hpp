#pragma once

#include <array>

#include <vulkan/vulkan.h>

#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/Pipeline.hpp"
#include "graphics/vulkan/QueueFamilyIndices.hpp"
#include "graphics/vulkan/vulkanSwapchain.hpp"

namespace flex {
enum struct QueueFamily;

struct VulkanRenderer {
private:
  static constexpr std::array<const char *, 1> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  static constexpr unsigned int mMaxFramesInFlight = 2;
  unsigned int mCurrentFrame;

  VkInstance mInstance{};
  vk::SurfaceKHR mSurface;

  VkPhysicalDevice mPhysicalDevice{};
  QueueFamilyIndices mQueueFamilyIndices;
  VkDevice mDevice{};

  VkQueue mGraphicsQueue{};
  VkQueue mPresentQueue{};
  VkQueue mTransferQueue{};

  Swapchain mSwapchain;
  Pipeline mPipeline{};
  VkRenderPass mRenderPass{};

  VkCommandPool mCommandPool{};
  std::vector<VkCommandBuffer> mCommandBuffers;

  std::array<VkSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores{};
  std::array<VkSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores{};
  std::array<VkFence, mMaxFramesInFlight> mInFlightFences{};
  std::vector<VkFence> mImagesInFlight;

  void createVulkanInstance(RenderWindow const &window);
  void selectPhysicalDevice();
  [[nodiscard]] unsigned int ratePhysicalDevice(VkPhysicalDevice const &physicalDevice,
                                                VkSurfaceKHR const &vulkanSurface) const;
  [[nodiscard]] bool
  physicalDeviceSupportsRequiredExtensions(VkPhysicalDevice const &physicalDevice) const;
  void createVulkanDevice();
  void retrieveQueues();
  void createRenderPass();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(RenderWindow const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;

  void draw();
};
} // namespace flex
