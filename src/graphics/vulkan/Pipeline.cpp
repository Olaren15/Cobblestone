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

Pipeline::Pipeline(vk::Device const &device, SwapChain const &swapChain,
                   vk::RenderPass const &renderPass) {
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

  vk::Viewport viewport{0.0f,
                        0.0f,
                        static_cast<float>(swapChain.extent.width),
                        static_cast<float>(swapChain.extent.height),
                        0.0f,
                        1.0f};
  vk::Rect2D scissor{{0, 0}, swapChain.extent};
  vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{{}, viewport, scissor};

  vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{{},
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

  vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{false,
                                                                  vk::BlendFactor::eOne,
                                                                  vk::BlendFactor::eZero,
                                                                  vk::BlendOp::eAdd,
                                                                  vk::BlendFactor::eOne,
                                                                  vk::BlendFactor::eZero,
                                                                  vk::BlendOp::eAdd,
                                                                  {}};

  constexpr std::array<float, 4> blendConstants{0.0f, 0.0f, 0.0f, 0.0f};
  vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
      {}, false, vk::LogicOp::eCopy, colorBlendAttachmentState, blendConstants};

  vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo{
      {}, vk::SampleCountFlagBits::e1, false, 1.0f, nullptr, false, false};

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
  device.destroyPipeline(pipeline);
  device.destroyPipelineLayout(pipelineLayout);
  device.destroyShaderModule(fragShaderModule);
  device.destroyShaderModule(vertShaderModule);
}
} // namespace flex
