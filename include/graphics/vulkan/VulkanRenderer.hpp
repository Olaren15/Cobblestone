#pragma once

#include <array>

#include <vulkan/vulkan.h>

#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"
#include "graphics/vulkan/VulkanFrame.hpp"
#include "graphics/vulkan/VulkanMemoryManager.hpp"
#include "graphics/vulkan/VulkanPipeline.hpp"
#include "graphics/vulkan/VulkanQueues.hpp"
#include "graphics/vulkan/VulkanSwapchain.hpp"

namespace flex {
enum struct QueueFamily;

struct VulkanRenderer {
private:
  struct {
    VulkanFrame *currentFrame{};
    unsigned int currentFrameNumber = 0;
    unsigned int imageIndex = 0;
    bool acquiredImageStillValid = false;
    bool doNotRender = false;
  } mState;

  static constexpr std::array<const char *, 1> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  RenderWindow const &mWindow;
  Camera const &mCamera;

  VkInstance mInstance{};
  VkSurfaceKHR mSurface;

  VkPhysicalDevice mPhysicalDevice{};
  VkDevice mDevice{};

  VulkanQueues mQueues;

  VulkanMemoryManager mMemoryManager;

  VkRenderPass mRenderPass{};
  VulkanSwapchain mSwapchain;

  VulkanPipeline mPipeline{};

  static constexpr unsigned int mMaxFramesInFlight = 2;
  std::array<VulkanFrame, mMaxFramesInFlight> mFrames;

  void createVulkanInstance();
  void selectPhysicalDevice();
  [[nodiscard]] static unsigned int ratePhysicalDevice(VkPhysicalDevice const &physicalDevice,
                                                       VkSurfaceKHR const &vulkanSurface);
  [[nodiscard]] static bool
  physicalDeviceSupportsRequiredExtensions(VkPhysicalDevice const &physicalDevice);
  void createVulkanDevice();
  void createRenderPass();
  void initialiseFrames();
  void handleFrameBufferResize();

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(RenderWindow const &window, Camera const &camera);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;

  bool acquireNextFrame();
  void startDraw();
  void drawMesh(Mesh &mesh);
  void endDraw();
  void present();

  void stop();
};
} // namespace flex
