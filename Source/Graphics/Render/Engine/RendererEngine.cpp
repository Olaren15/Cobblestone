#include "RendererEngine.hpp"

#include "Core/Time/Time.hpp"
#include "Graphics/CommandBufferRecorder/CommandBufferRecorder.hpp"
#include "Graphics/Materials/ChunkMaterial/ChunkMaterial.hpp"
#include "Graphics/Shaders/DefaultShader/DefaultShader.hpp"
#include "Graphics/Utils/VulkanHelpers.hpp"

namespace cbl::gfx {
RendererEngine::RendererEngine()
    : mWindow{}, mGPU{mWindow}, mMemoryManager{mGPU},
      mSwapchain{mGPU, mWindow, mMemoryManager}, mFrames{Frame{mGPU}, Frame{mGPU}} {

  mState.currentFrame = &mFrames[mState.currentFrameNumber];
}

bool RendererEngine::acquireNextFrame() {
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

void RendererEngine::present() {

  VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

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

void RendererEngine::drawScene() {
  if (mState.currentScene == nullptr) {
    return;
  }

  if (!mState.shouldRender || !acquireNextFrame()) {
    if (mSwapchain.isNotZeroPixels(mWindow)) {
      mSwapchain.handleFrameBufferResize(mWindow);
    }
    return;
  }

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

  recorder.endRenderPass().end();

  present();
}

void RendererEngine::update() {
  Time::tick();
  mWindow.update();
  mState.currentScene->update();
  drawScene();
}

bool RendererEngine::isRunning() { return mWindow.isOpen(); }

void RendererEngine::loadWorld(World &scene) {
  if (mState.currentScene != nullptr) {
    unloadWorld();
  }

  mState.currentScene = &scene;

  for (Mesh &mesh : mState.currentScene->meshes) {
    if (!mesh.buffer.isValid) {
      mMemoryManager.generateMeshBuffer(mesh);
    }
  }

  mState.currentScene->shaders.push_back(new DefaultShader{mGPU, mSwapchain.renderPass});
  mState.currentScene->materials.push_back(
      new ChunkMaterial{mGPU, mMemoryManager, mState.currentScene->shaders[0]});
}

void RendererEngine::unloadWorld() {
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
