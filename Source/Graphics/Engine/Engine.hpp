#pragma once

#include <array>

#include <vulkan/vulkan.h>

#include "Core/World/World.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Frame/Frame.hpp"
#include "Graphics/GPU/GPU.hpp"
#include "Graphics/Memory/Buffer/Buffer.hpp"
#include "Graphics/Memory/MemoryManager/MemoryManager.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Swapchain/Swapchain.hpp"
#include "Graphics/Window/Window.hpp"

namespace cbl::gfx {

struct Engine {
private:
  struct {
    World *currentScene = nullptr;
    Frame *currentFrame = nullptr;
    unsigned int currentFrameNumber = 0;
    unsigned int imageIndex = 0;
    bool shouldRender = true;
  } mState;

  Window mWindow;

  GPU mGPU;
  mem::MemoryManager mMemoryManager;

  Swapchain mSwapchain;

  static constexpr unsigned int mMaxFramesInFlight = 2;
  std::array<Frame, mMaxFramesInFlight> mFrames;

  VkDescriptorPool imguiPool;
  void initImgui();

  bool acquireNextFrame();
  void drawScene();

public:
  Engine();
  Engine(Engine const &) = delete;
  ~Engine();

  void operator=(Engine const &) = delete;
  void operator=(Engine) = delete;

  void run();

  [[nodiscard]] bool isRunning();

  void loadWorld(World &scene);
  void unloadWorld();
};
} // namespace cbl::gfx
