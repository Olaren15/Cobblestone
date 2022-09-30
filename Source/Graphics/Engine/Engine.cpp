#include "Engine.hpp"

#include "External/imgui/backends/imgui_impl_vulkan.h"
#include "External/imgui/imgui.h"

#include "Core/Time/Time.hpp"
#include "Graphics/CommandBufferRecorder/CommandBufferRecorder.hpp"
#include "Graphics/Materials/ChunkMaterial/ChunkMaterial.hpp"
#include "Graphics/Shaders/ChunkShader/ChunkShader.hpp"
#include "Graphics/Utils/VulkanHelpers.hpp"

namespace cbl::gfx {
Engine::Engine()
    : mWindow{}, mGPU{mWindow}, mMemoryManager{mGPU},
      mSwapchain{mGPU, mWindow, mMemoryManager}, mFrames{Frame{mGPU}, Frame{mGPU}} {

  mState.currentFrame = &mFrames[mState.currentFrameNumber];
  initImgui();
}

Engine::~Engine() {
  vkDestroyDescriptorPool(mGPU.device, imguiPool, nullptr);
  ImGui_ImplVulkan_Shutdown();
}

void Engine::initImgui() {
  // 1: create descriptor pool for IMGUI
  VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                       {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                       {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                       {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                       {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                       {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                       {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                       {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                       {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                       {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                       {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000;
  pool_info.poolSizeCount = std::size(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;

  validateVkResult(vkCreateDescriptorPool(mGPU.device, &pool_info, nullptr, &imguiPool));

  // 2: initialize imgui library
  ImGui::CreateContext();
  mWindow.initImgui();

  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = mGPU.instance;
  init_info.PhysicalDevice = mGPU.physicalDevice;
  init_info.Device = mGPU.device;
  init_info.Queue = mGPU.graphicsQueue;
  init_info.DescriptorPool = imguiPool;
  init_info.MinImageCount = static_cast<uint32_t>(mSwapchain.frameBufferImages.size());
  init_info.ImageCount = static_cast<uint32_t>(mSwapchain.frameBufferImages.size());

  ImGui_ImplVulkan_Init(&init_info, mSwapchain.renderPass);

  CommandBufferRecorder recorder{mState.currentFrame->commandBuffer};
  recorder.beginOneTime();
  ImGui_ImplVulkan_CreateFontsTexture(mState.currentFrame->commandBuffer);
  recorder.end().submit(mGPU.graphicsQueue);
}

bool Engine::acquireNextFrame() {
  validateVkResult(vkWaitForFences(mGPU.device, 1, &mState.currentFrame->renderFinishedFence,
                                   VK_TRUE, UINT64_MAX));

  if (VkResult result = vkAcquireNextImageKHR(mGPU.device, mSwapchain.swapchain, UINT64_MAX,
                                              mState.currentFrame->imageAvailableSemaphore,
                                              VK_NULL_HANDLE, &mState.imageIndex);
      result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    return false;
  } else {
    // Image acquired
    validateVkResult(result);
  }

  validateVkResult(vkResetFences(mGPU.device, 1, &mState.currentFrame->renderFinishedFence));
  return true;
}

void Engine::drawScene() {
  if (mState.currentScene == nullptr) {
    return;
  }

  if (!mState.shouldRender || !acquireNextFrame()) {
    if (mSwapchain.isValid(mWindow)) {
      mSwapchain.handleFrameBufferResize(mWindow);
      ImGui::EndFrame();
    }
    return;
  }

  ImGui::Render();

  VkRect2D renderArea;
  renderArea.offset = {0, 0};
  renderArea.extent = mSwapchain.frameBufferImages[0].extent;

  CommandBufferRecorder recorder{mState.currentFrame->commandBuffer};
  recorder.beginOneTime()
      .setViewPort(renderArea.extent)
      .setScissor(renderArea)
      .beginRenderPass(mSwapchain.renderPass, mSwapchain.framebuffers[mState.imageIndex],
                       renderArea);

  for (BaseShader const *shader : mState.currentScene->shaders) {
    if (!shader) {
      continue;
    }

    recorder
        .bindGraphicsShader(*shader) //
        .pushCameraView(mState.currentScene->camera.getViewMatrix(mSwapchain.getAspectRatio()),
                        *shader);

    for (BaseMaterial const *material : mState.currentScene->materials) {
      if (!material) {
        continue;
      }

      recorder.bindMaterial(*shader, *material);

      for (Mesh const &mesh : mState.currentScene->meshes) {
        recorder
            .pushModelPosition(mesh.position, *shader) //
            .drawMesh(mesh);
      }
    }
  }

  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mState.currentFrame->commandBuffer);

  recorder.endRenderPass().end();

  constexpr VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pWaitDstStageMask = &waitStage;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &mState.currentFrame->imageAvailableSemaphore;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &mState.currentFrame->renderFinishedSemaphore;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &mState.currentFrame->commandBuffer;

  validateVkResult(
      vkQueueSubmit(mGPU.graphicsQueue, 1, &submitInfo, mState.currentFrame->renderFinishedFence));

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &mState.currentFrame->renderFinishedSemaphore;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &mSwapchain.swapchain;
  presentInfo.pImageIndices = &mState.imageIndex;

  validateVkResult(vkQueuePresentKHR(mGPU.presentQueue, &presentInfo));

  mState.currentFrame = &mFrames[++mState.currentFrameNumber %= mMaxFramesInFlight];
}

void Engine::run() {
  while (mWindow.isOpen()) {
    Time::tick();
    ImGui_ImplVulkan_NewFrame();
    mWindow.update();
    ImGui::NewFrame();
    ImGui::ShowMetricsWindow();

    mState.currentScene->update();
    drawScene();
  }
}

bool Engine::isRunning() { return mWindow.isOpen(); }

void Engine::loadWorld(World &scene) {
  if (mState.currentScene != nullptr) {
    unloadWorld();
  }

  mState.currentScene = &scene;

  for (Mesh &mesh : mState.currentScene->meshes) {
    if (!mesh.buffer.isValid) {
      mMemoryManager.generateMeshBuffer(mesh);
    }
  }

  mState.currentScene->shaders.push_back(new ChunkShader{mGPU, mSwapchain.renderPass});
  mState.currentScene->materials.push_back(
      new ChunkMaterial{mGPU, mMemoryManager, mState.currentScene->shaders[0]});
}

void Engine::unloadWorld() {
  if (mState.currentScene == nullptr) {
    return;
  }

  mGPU.waitIdle();

  for (Mesh &mesh : mState.currentScene->meshes) {
    if (mesh.buffer.isValid) {
      mesh.buffer.memoryManager->destroyBuffer(mesh.buffer);
    }
  }

  for (BaseMaterial *material : mState.currentScene->materials) {
    delete material;
  }

  for (BaseShader *shader : mState.currentScene->shaders) {
    delete shader;
  }

  mState.currentScene = nullptr;
}
} // namespace cbl::gfx
