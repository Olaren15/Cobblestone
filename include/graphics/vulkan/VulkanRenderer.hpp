#pragma once

#include <array>

#include <vulkan/vulkan.h>

#include "core/Scene.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/RenderWindow.hpp"
#include "graphics/vulkan/VulkanBuffer.hpp"
#include "graphics/vulkan/VulkanFrame.hpp"
#include "graphics/vulkan/VulkanGPU.hpp"
#include "graphics/vulkan/VulkanMemoryManager.hpp"
#include "graphics/vulkan/VulkanSwapchain.hpp"

namespace flex {
enum struct QueueFamily;

struct VulkanRenderer {
private:
  struct {
    Scene *currentScene = nullptr;
    VulkanFrame *currentFrame = nullptr;
    unsigned int currentFrameNumber = 0;
    unsigned int imageIndex = 0;
    bool shouldRender = true;
  } mState;

  RenderWindow const &mWindow;

  VulkanGPU mGPU{};
  VulkanMemoryManager mMemoryManager;

  VulkanSwapchain mSwapchain{};

  VkRenderPass mRenderPass{};
  VkPipelineLayout mPipelineLayout{};

  static constexpr unsigned int mMaxFramesInFlight = 2;
  std::array<VulkanFrame, mMaxFramesInFlight> mFrames;

  void createRenderPass();
  void createPipelineLayout();

  bool acquireNextFrame();
  void present();

public:
  VulkanRenderer() = delete;
  VulkanRenderer(VulkanRenderer const &) = delete;
  explicit VulkanRenderer(RenderWindow const &window);
  ~VulkanRenderer();

  void operator=(VulkanRenderer const &) = delete;
  void operator=(VulkanRenderer) = delete;

  void drawScene();

  void loadScene(Scene &scene, std::vector<VulkanShaderInformation *> &shadersInfo);
  void unloadScene();
};
} // namespace flex
