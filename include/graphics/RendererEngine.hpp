#pragma once

#include <array>

#include <vulkan/vulkan.h>

#include "Buffer.hpp"
#include "Frame.hpp"
#include "GPU.hpp"
#include "MemoryManager.hpp"
#include "Swapchain.hpp"
#include "core/Scene.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/RenderWindow.hpp"

namespace flex {
enum struct QueueFamily;

struct RendererEngine {
private:
  struct {
    Scene *currentScene = nullptr;
    Frame *currentFrame = nullptr;
    unsigned int currentFrameNumber = 0;
    unsigned int imageIndex = 0;
    bool shouldRender = true;
  } mState;

  RenderWindow mWindow;

  GPU mGPU;
  MemoryManager mMemoryManager;

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

  void loadScene(Scene &scene, std::vector<ShaderInformation *> &shadersInfo);
  void unloadScene();
};
} // namespace flex
