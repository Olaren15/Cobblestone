#pragma once

#include <array>

#include <vulkan/vulkan.h>

#include "core/Scene.hpp"
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
  // constants
  std::vector<const char *> mRequiredDeviceExtensionsNames{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };
  static constexpr unsigned int mMaxFramesInFlight = 2;

  struct {
    Scene *currentScene = nullptr;
    VulkanFrame *currentFrame = nullptr;
    unsigned int currentFrameNumber = 0;
    unsigned int imageIndex = 0;
    bool acquiredImageStillValid = false;
    bool doNotRender = false;
  } mState;

  RenderWindow const &mWindow;

  VkInstance mInstance{};
  VkSurfaceKHR mSurface;

  VkPhysicalDevice mPhysicalDevice{};
  VkDevice mDevice{};

  VulkanQueues mQueues;

  VulkanMemoryManager mMemoryManager;

  VkRenderPass mRenderPass{};
  VulkanSwapchain mSwapchain;

  VulkanPipeline mPipeline{};

  std::array<VulkanFrame, mMaxFramesInFlight> mFrames;

  void createVulkanInstance();
  void selectPhysicalDevice();
  void createVulkanDevice();
  void createRenderPass();
  void initialiseFrames();
  void handleFrameBufferResize();

  bool acquireNextFrame();
  void startDraw();
  void drawMesh(Mesh &mesh) const;
  void endDraw() const;
  void present();

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(RenderWindow const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;

  void drawScene();

  void loadScene(Scene &scene);
  void unloadScene();
};
} // namespace flex
