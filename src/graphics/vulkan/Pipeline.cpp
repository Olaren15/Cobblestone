#include "graphics/vulkan/Pipeline.hpp"

#include <fstream>
#include <stdexcept>

namespace flex {
vk::ShaderModule Pipeline::createShaderModule(vk::Device const &device,
                                              std::filesystem::path const &shaderPath) const {
  std::ifstream shaderFile{shaderPath.string(), std::ios::ate | std::ios::binary};

  if (!shaderFile.is_open()) {
    throw std::runtime_error{"Failed to open file " + shaderPath.string()};
  }

  std::streamsize const fileSize = shaderFile.tellg();
  std::vector<char> buffer(static_cast<size_t>(fileSize));

  shaderFile.seekg(0);
  shaderFile.read(buffer.data(), fileSize);
  shaderFile.close();

  vk::ShaderModuleCreateInfo const moduleCreateInfo{
      {}, buffer.size(), reinterpret_cast<const uint32_t *>(buffer.data())};

  return device.createShaderModule(moduleCreateInfo);
}

vk::PipelineViewportStateCreateInfo
Pipeline::buildViewPortStateCreateInfo(vk::Extent2D const &swapChainExtent) {

  vk::Viewport viewport{0.0f,
                        0.0f,
                        static_cast<float>(swapChainExtent.width),
                        static_cast<float>(swapChainExtent.height),
                        0.0f,
                        1.0f};
  vk::Rect2D scissor{{0, 0}, swapChainExtent};
  return vk::PipelineViewportStateCreateInfo{{}, viewport, scissor};
}

vk::PipelineRasterizationStateCreateInfo Pipeline::buildRasterizationStateCreateInfo() {
  return vk::PipelineRasterizationStateCreateInfo{{},
                                                  false,
                                                  false,
                                                  vk::PolygonMode::eFill,
                                                  vk::CullModeFlagBits::eBack,
                                                  vk::FrontFace::eClockwise,
                                                  false,
                                                  0.0f,
                                                  0.0f,
                                                  0.0f,
                                                  1.0f};
}

vk::PipelineColorBlendStateCreateInfo Pipeline::buildColorBlendStateCreateInfo() {
  vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{false,
                                                                  vk::BlendFactor::eOne,
                                                                  vk::BlendFactor::eZero,
                                                                  vk::BlendOp::eAdd,
                                                                  vk::BlendFactor::eOne,
                                                                  vk::BlendFactor::eZero,
                                                                  vk::BlendOp::eAdd,
                                                                  {}};
  constexpr std::array<float, 4> blendConstants{0.0f, 0.0f, 0.0f, 0.0f};
  return vk::PipelineColorBlendStateCreateInfo{
      {}, false, vk::LogicOp::eCopy, colorBlendAttachmentState, blendConstants};
}

vk::RenderPass Pipeline::createRenderPass(vk::Device const &device,
                                          vk::Format const &swapChainFormat) {
  vk::AttachmentDescription colorAttachment{{},
                                            swapChainFormat,
                                            vk::SampleCountFlagBits::e1,
                                            vk::AttachmentLoadOp::eClear,
                                            vk::AttachmentStoreOp::eDontCare,
                                            vk::AttachmentLoadOp::eDontCare,
                                            vk::AttachmentStoreOp::eDontCare,
                                            vk::ImageLayout::eUndefined,
                                            vk::ImageLayout::ePresentSrcKHR};

  vk::AttachmentReference colorAttachmentReference{0, vk::ImageLayout::eColorAttachmentOptimal};

  vk::SubpassDescription subpassDescription{
      {}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentReference, {}, {}, {}};

  vk::RenderPassCreateInfo const renderPassCreateInfo{{}, colorAttachment, subpassDescription, {}};
  return device.createRenderPass((renderPassCreateInfo));
}

Pipeline::Pipeline(vk::Device const &device, SwapChain const &swapChain) {
  vertShaderModule = createShaderModule(device, std::filesystem::path{"shaders/vert.spv"});
  fragShaderModule = createShaderModule(device, std::filesystem::path{"shaders/frag.spv"});

  std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages{
      vk::PipelineShaderStageCreateInfo{
          {}, vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main"},
      vk::PipelineShaderStageCreateInfo{
          {}, vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main"}};

  vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{{}, {}, {}};
  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{
      {}, vk::PrimitiveTopology::eTriangleList, false};

  vk::PipelineViewportStateCreateInfo viewportStateCreateInfo =
      buildViewPortStateCreateInfo(swapChain.extent);
  vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
      buildRasterizationStateCreateInfo();
  vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo =
      buildColorBlendStateCreateInfo();

  vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo{
      {}, vk::SampleCountFlagBits::e1, false, 1.0f, nullptr, false, false};

  renderPass = createRenderPass(device, swapChain.format);

  pipelineLayout = device.createPipelineLayout({});
  vk::GraphicsPipelineCreateInfo pipelineCreateInfo{{},
                                                    shaderStages,
                                                    &vertexInputStateCreateInfo,
                                                    &inputAssemblyStateCreateInfo,
                                                    {},
                                                    &viewportStateCreateInfo,
                                                    &rasterizationStateCreateInfo,
                                                    &multiSampleStateCreateInfo,
                                                    nullptr,
                                                    &colorBlendStateCreateInfo,
                                                    nullptr,
                                                    pipelineLayout,
                                                    renderPass,
                                                    0,
                                                    {},
                                                    -1};

  pipeline = device.createGraphicsPipeline({}, pipelineCreateInfo).value;
}

void Pipeline::destroy(vk::Device const &device) const {
  device.destroyPipelineLayout(pipelineLayout);
  device.destroyRenderPass(renderPass);
  device.destroyShaderModule(fragShaderModule);
  device.destroyShaderModule(vertShaderModule);
}
} // namespace flex
