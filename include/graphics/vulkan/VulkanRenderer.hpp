#pragma once

#include "VulkanBuffer.hpp"

#include <array>

#include <vulkan/vulkan.h>

#include "graphics/RenderWindow.hpp"
#include "graphics/Vertex.hpp"
#include "graphics/vulkan/VulkanPipeline.hpp"
#include "graphics/vulkan/VulkanQueueFamilyIndices.hpp"
#include "graphics/vulkan/VulkanSwapchain.hpp"
#include <graphics/vulkan/vk_mem_alloc.h>

namespace flex {
enum struct QueueFamily;

struct VulkanRenderer {
private:
  // temporary
  std::vector<flex::Vertex> mVertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  static constexpr std::array<const char *, 1> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  static constexpr unsigned int mMaxFramesInFlight = 2;
  unsigned int mCurrentFrame;
  bool mDoNotRender = false;

  RenderWindow *mWindow;

  VkInstance mInstance{};
  VkSurfaceKHR mSurface;

  VkPhysicalDevice mPhysicalDevice{};
  VulkanQueueFamilyIndices mQueueFamilyIndices;
  VkDevice mDevice{};

  VkQueue mGraphicsQueue{};
  VkQueue mPresentQueue{};
  VkQueue mTransferQueue{};

  VmaAllocator mAllocator{};

  VulkanSwapchain mSwapchain;
  VulkanPipeline mPipeline{};
  VkRenderPass mRenderPass{};

  VkCommandPool mCommandPool{};
  std::vector<VkCommandBuffer> mCommandBuffers;

  std::array<VkSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores{};
  std::array<VkSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores{};
  std::array<VkFence, mMaxFramesInFlight> mInFlightFences{};
  std::vector<VkFence> mImagesInFlight;

  VulkanBuffer mVertexBuffer;

  void createVulkanInstance();
  void selectPhysicalDevice();
  [[nodiscard]] unsigned int ratePhysicalDevice(VkPhysicalDevice const &physicalDevice,
                                                VkSurfaceKHR const &vulkanSurface) const;
  [[nodiscard]] bool
  physicalDeviceSupportsRequiredExtensions(VkPhysicalDevice const &physicalDevice) const;
  void createVulkanDevice();
  void retrieveQueues();
  void createAllocator();
  void createRenderPass();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
  void createVertexBuffer();

  void recordCommandBuffer(uint32_t &imageIndex);

  void handleFrameBufferResize();

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(RenderWindow &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;

  void draw();
};
} // namespace flex
