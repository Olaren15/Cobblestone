﻿#include "graphics/RendererEngine.hpp"

#include <map>

#include "graphics/CommandBufferRecorder.hpp"
#include "core/Time.hpp"

namespace flex {
RendererEngine::RendererEngine() {
  mGPU.initialise(mWindow);
  mMemoryManager.initialise(mGPU);
  createRenderPass();
  createPipelineLayout();
  mSwapchain.initialise(mGPU, mWindow, mRenderPass, mMemoryManager);
  for (Frame &frame : mFrames) {
    frame.initialise(mGPU);
  }
  mState.currentFrame = &mFrames[mState.currentFrameNumber];
}

RendererEngine::~RendererEngine() {
  mGPU.waitIdle();

  for (Frame &frame : mFrames) {
    frame.destroy(mGPU);
  }

  mSwapchain.destroy(mGPU);
  vkDestroyPipelineLayout(mGPU.device, mPipelineLayout, nullptr);
  vkDestroyRenderPass(mGPU.device, mRenderPass, nullptr);
  mMemoryManager.destroy();
  mGPU.destroy();
}

void RendererEngine::createRenderPass() {

  SwapchainSupportDetails const swapchainSupportDetails = SwapchainSupportDetails{mGPU};

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format =
      Swapchain::getSupportedSwapchainSurfaceFormat(swapchainSupportDetails).format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentReference;
  colorAttachmentReference.attachment = 0;
  colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = Swapchain::getSupportedDepthBufferFormat(mGPU);
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentReference{};
  depthAttachmentReference.attachment = 1;
  depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDescription{};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &colorAttachmentReference;
  subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

  VkSubpassDependency subpassDependency{};
  subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependency.dstSubpass = 0;
  subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.srcAccessMask = 0;
  subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassCreateInfo.pAttachments = attachments.data();
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpassDescription;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &subpassDependency;

  validateVkResult(vkCreateRenderPass(mGPU.device, &renderPassCreateInfo, nullptr, &mRenderPass));
}

void RendererEngine::createPipelineLayout() {
  VkPushConstantRange pushConstantRange;
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(glm::mat4) * 2;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
  pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

  validateVkResult(
      vkCreatePipelineLayout(mGPU.device, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout));
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
    if (mSwapchain.canBeResized(mGPU, mWindow)) {
      mSwapchain.handleFrameBufferResize(mGPU, mWindow, mRenderPass);
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
      .beginRenderPass(mRenderPass, mSwapchain.framebuffers[mState.imageIndex], renderArea)
      .pushCameraView(mState.currentScene->camera.getViewMatrix(mSwapchain.getAspectRatio()),
                      mPipelineLayout);

  for (Shader &shader : mState.currentScene->shaders) {
    recorder.bindPipeline(shader.pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);

    for (Mesh &mesh : mState.currentScene->meshes) {
      if (mesh.shaderId == shader.shaderId) {
        recorder
            .pushModelPosition(mesh.position, mPipelineLayout) //
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

bool RendererEngine::isRunning() {
  return mWindow.isOpen();
}


void RendererEngine::loadScene(Scene &scene, std::vector<ShaderInformation *> &shadersInfo) {
  if (mState.currentScene != nullptr) {
    unloadScene();
  }

  mState.currentScene = &scene;

  for (Mesh &mesh : mState.currentScene->meshes) {
    if (!mesh.buffer.isValid) {
      mMemoryManager.generateMeshBuffer(mesh);
    }
  }

  for (ShaderInformation *shaderInfo : shadersInfo) {
    Shader shader{mGPU, mRenderPass, mPipelineLayout, *shaderInfo};
    mState.currentScene->shaders.push_back(shader);
  }
}

void RendererEngine::unloadScene() {
  if (mState.currentScene == nullptr) {
    return;
  }

  mGPU.waitIdle();

  for (Mesh &mesh : mState.currentScene->meshes) {
    if (mesh.buffer.isValid) {
      mesh.buffer.memoryManager->destroyBuffer(mesh.buffer);
    }
  }

  for (Shader &shader : mState.currentScene->shaders) {
    shader.destroy(mGPU);
  }
}

} // namespace flex