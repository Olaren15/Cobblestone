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
  static constexpr std::array<const char *, 1> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  static constexpr unsigned int mMaxFramesInFlight = 2;
  unsigned int mCurrentFrame;

  vk::Instance mInstance;
  vk::SurfaceKHR mSurface;

  vk::PhysicalDevice mPhysicalDevice;
  QueueFamilyIndices mQueueFamilyIndices;
  vk::Device mDevice;

  vk::Queue mGraphicsQueue;
  vk::Queue mPresentQueue;
  vk::Queue mTransferQueue;

  SwapChain mSwapChain;
  Pipeline mPipeline;
  vk::RenderPass mRenderPass;

  vk::CommandPool mCommandPool;
  std::vector<vk::CommandBuffer> mCommandBuffers;

  std::array<vk::Semaphore, mMaxFramesInFlight> mImageAvailableSemaphores;
  std::array<vk::Semaphore, mMaxFramesInFlight> mRenderFinishedSemaphores;
  std::array<vk::Fence, mMaxFramesInFlight> mInFlightFences;
  std::vector<vk::Fence> mImagesInFlight;

  void createVulkanInstance(Window const &window);
  void selectPhysicalDevice();
  [[nodiscard]] unsigned int ratePhysicalDevice(vk::PhysicalDevice const &physicalDevice,
                                                vk::SurfaceKHR const &vulkanSurface) const;
  [[nodiscard]] bool
  physicalDeviceSupportsRequiredExtensions(vk::PhysicalDevice const &physicalDevice) const;
  void createVulkanDevice();
  void retrieveQueues();
  void createRenderPass();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(Window const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;

  void draw();
};
} // namespace flex
