#include "graphics/vulkan/VulkanRenderer.hpp"

#include <map>

namespace flex {
VulkanRenderer::VulkanRenderer(RenderWindow const &window)
    : mWindow(window), mSwapchain(mMemoryManager) {
  if (window.getRenderAPI() != RenderAPI::Vulkan) {
    throw InvalidRenderAPIException{
        "Can't create vulkan renderer if window is not initialized with the "
        "Vulkan render API"};
  }

  mGPU.initialize(window);
  mMemoryManager.initialize(mGPU);
  createRenderPass();
  mSwapchain.createSwapchain(mGPU, window, mRenderPass, mMemoryManager);
  mPipeline.createPipeline(mGPU, mRenderPass);
  for (VulkanFrame &frame : mFrames) {
    frame.initialise(mGPU);
  }
  mState.currentFrame = &mFrames[mState.currentFrameNumber];
}

VulkanRenderer::~VulkanRenderer() {
  mGPU.waitIdle();

  for (VulkanFrame &frame : mFrames) {
    frame.destroy(mGPU);
  }

  mPipeline.destroy(mGPU);
  mSwapchain.destroy(mGPU);
  vkDestroyRenderPass(mGPU.device, mRenderPass, nullptr);
  mMemoryManager.destroy();
  mGPU.destroy();
}

void VulkanRenderer::createRenderPass() {

  VulkanSwapchainSupportDetails const swapchainSupportDetails = VulkanSwapchainSupportDetails{mGPU};

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format =
      VulkanSwapchain::getSupportedSwapchainSurfaceFormat(swapchainSupportDetails).format;
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
  depthAttachment.format = VulkanSwapchain::getSupportedDepthBufferFormat(mGPU);
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

void VulkanRenderer::handleFrameBufferResize() {
  if (mWindow.hasFocus()) {
    mState.doNotRender = false;

    mGPU.waitIdle();
    mSwapchain.handleFrameBufferResize(mGPU, mWindow, mRenderPass);
  } else {
    mState.doNotRender = true;
  }
}

bool VulkanRenderer::acquireNextFrame() {
  if (mState.doNotRender) {
    // check if we can render again
    handleFrameBufferResize();
    if (mState.doNotRender) {
      return false;
    }
  }

  if (VkResult fenceStatus =
          vkGetFenceStatus(mGPU.device, mState.currentFrame->renderFinishedFence);
      fenceStatus == VK_NOT_READY) {
    // fence is not ready
    return false;
  } else {
    validateVkResult(fenceStatus);
  }

  // do no try to acquire another image if we already have a valid one
  if (!mState.acquiredImageStillValid) {
    if (VkResult result = vkAcquireNextImageKHR(mGPU.device, mSwapchain.swapchain, 0,
                                                mState.currentFrame->imageAvailableSemaphore,
                                                VK_NULL_HANDLE, &mState.imageIndex);
        result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      handleFrameBufferResize();
      // return immediately because the acquired frame was not valid
      return false;
    } else if (result == VK_NOT_READY || result == VK_TIMEOUT) {
      // Since we do not wait, it is possible that the next image is not ready
      return false;
    } else {
      // Image acquired
      validateVkResult(result);
      mState.acquiredImageStillValid = true;
    }
  }

  return true;
}

void VulkanRenderer::startDraw() {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  validateVkResult(vkBeginCommandBuffer(mState.currentFrame->commandBuffer, &beginInfo));

  std::array<VkViewport, 1> viewport{};
  viewport[0].x = 0.0f;
  viewport[0].y = 0.0f;
  viewport[0].width = static_cast<float>(mSwapchain.frameBufferImages[0].extent.width);
  viewport[0].height = static_cast<float>(mSwapchain.frameBufferImages[0].extent.height);
  viewport[0].minDepth = 0.0f;
  viewport[0].maxDepth = 1.0f;

  std::array<VkRect2D, 1> scissors{};
  scissors[0].offset = {0, 0};
  scissors[0].extent = mSwapchain.frameBufferImages[0].extent;

  // dynamic states
  vkCmdSetViewport(mState.currentFrame->commandBuffer, 0, static_cast<uint32_t>(viewport.size()),
                   viewport.data());
  vkCmdSetScissor(mState.currentFrame->commandBuffer, 0, static_cast<uint32_t>(scissors.size()),
                  scissors.data());

  VkRect2D renderArea;
  renderArea.offset = {0, 0};
  renderArea.extent = mSwapchain.frameBufferImages[0].extent;

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = mRenderPass;
  renderPassBeginInfo.framebuffer = mSwapchain.framebuffers[mState.imageIndex];
  renderPassBeginInfo.renderArea = renderArea;
  renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassBeginInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(mState.currentFrame->commandBuffer, &renderPassBeginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  float const aspectRatio = static_cast<float>(mSwapchain.frameBufferImages[0].extent.width) /
                            static_cast<float>(mSwapchain.frameBufferImages[0].extent.height);
  glm::mat4 cameraView = mState.currentScene->camera.getViewMatrix(aspectRatio);
  vkCmdPushConstants(mState.currentFrame->commandBuffer, mPipeline.pipelineLayout,
                     VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof cameraView, &cameraView);

  vkCmdBindPipeline(mState.currentFrame->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    mPipeline.pipeline);
}

void VulkanRenderer::drawMesh(Mesh &mesh) const {

  vkCmdBindIndexBuffer(mState.currentFrame->commandBuffer, mesh.vulkanBuffer->buffer, 0,
                       VK_INDEX_TYPE_UINT32);

  std::array<VkDeviceSize, 1> vertexBufferOffset{mesh.getIndicesSize()};
  vkCmdBindVertexBuffers(mState.currentFrame->commandBuffer, 0, 1, &mesh.vulkanBuffer->buffer,
                         vertexBufferOffset.data());

  vkCmdDrawIndexed(mState.currentFrame->commandBuffer, static_cast<uint32_t>(mesh.indices.size()),
                   1, 0, 0, 0);
}

void VulkanRenderer::endDraw() const {
  vkCmdEndRenderPass(mState.currentFrame->commandBuffer);
  validateVkResult(vkEndCommandBuffer(mState.currentFrame->commandBuffer));
}

void VulkanRenderer::present() {

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

  validateVkResult(vkResetFences(mGPU.device, 1, &mState.currentFrame->renderFinishedFence));
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

  // allows us to start preparing for the next frame
  mState.acquiredImageStillValid = false;
}

void VulkanRenderer::drawScene() {
  if (mState.currentScene == nullptr) {
    return;
  }

  while (!acquireNextFrame())
    ;
  startDraw();
  for (Mesh &mesh : mState.currentScene->meshes) {
    drawMesh(mesh);
  }
  endDraw();
  present();
}

void VulkanRenderer::loadScene(Scene &scene) {
  if (mState.currentScene != nullptr) {
    unloadScene();
  }

  mState.currentScene = &scene;

  for (Mesh &mesh : mState.currentScene->meshes) {
    if (!mesh.vulkanBuffer.has_value()) {
      mesh.vulkanBuffer = mMemoryManager.createMeshBuffer(mesh);
    }
  }
}

void VulkanRenderer::unloadScene() {
  if (mState.currentScene == nullptr) {
    return;
  }

  mGPU.waitIdle();

  for (Mesh &mesh : mState.currentScene->meshes) {
    mesh.vulkanBuffer->memoryManager.destroyBuffer(mesh.vulkanBuffer.value());
  }
}

} // namespace flex
