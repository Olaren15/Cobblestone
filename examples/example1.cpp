#include <iostream>

#include <FlexEngine.hpp>

int flexMain() {
  const flex::Configuration config;

  const unsigned int width = config.getUnsignedInt("display.window.width");
  const unsigned int height = config.getUnsignedInt("display.window.height");
  const bool fullscreen = config.getBool("display.window.fullscreen");

  flex::RenderWindow window{"window", width, height, fullscreen};
  flex::VulkanRenderer vulkanRenderer{window};

  flex::Mesh quad{{0, 1, 2, 2, 3, 0},
                  {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                   {{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                   {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                   {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}}};
  flex::VulkanBuffer quadBuffer = vulkanRenderer.createMeshBuffer(quad);

  flex::Mesh triangle{{0, 1, 2, 2, 3, 0},
                      {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                       {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                       {{-1.f, 0.0f}, {0.0f, 0.0f, 1.0f}}}};
  flex::VulkanBuffer triangleBuffer = vulkanRenderer.createMeshBuffer(triangle);

  unsigned int i = 0;

  while (!window.shouldExit()) {
    window.update();

    if (flex::InputHandler::keyPressed("Space")) {
      std::cout << i++ << '\n';
    }

    if (vulkanRenderer.acquireNextFrame()) {
      vulkanRenderer.startDraw();
      vulkanRenderer.drawMesh(quad, quadBuffer);
      vulkanRenderer.drawMesh(triangle, triangleBuffer);
      vulkanRenderer.endDraw();
      vulkanRenderer.present();
    }
  }

  vulkanRenderer.stop();

  vulkanRenderer.destroyMeshBuffer(quadBuffer);
  vulkanRenderer.destroyMeshBuffer(triangleBuffer);

  return EXIT_SUCCESS;
}
