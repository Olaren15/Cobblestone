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
#include "Graphics/Render/Window/RenderWindow.hpp"
#include "Graphics/Swapchain/Swapchain.hpp"

namespace cbl::gfx {
enum struct QueueFamily;

struct RendererEngine {
private:
  struct {
    World *currentScene = nullptr;
    Frame *currentFrame = nullptr;
    unsigned int currentFrameNumber = 0;
    unsigned int imageIndex = 0;
    bool shouldRender = true;
  } mState;

  RenderWindow mWindow;

  GPU mGPU;
  mem::MemoryManager mMemoryManager;

  Swapchain mSwapchain;

  static constexpr unsigned int mMaxFramesInFlight = 2;
  std::array<Frame, mMaxFramesInFlight> mFrames;

  bool acquireNextFrame();
  void present();

  void drawScene();

public:
  RendererEngine();
  RendererEngine(RendererEngine const &) = delete;

  void operator=(RendererEngine const &) = delete;
  void operator=(RendererEngine) = delete;

  void update();
  [[nodiscard]] bool isRunning();

  void loadWorld(World &scene);
  void unloadWorld();
};
} // namespace cbl::gfx
