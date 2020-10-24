#include <FlexEngine.hpp>

int flexMain() {
  const flex::Configuration config;

  const unsigned int width = config.getUnsignedInt("display.window.width");
  const unsigned int height = config.getUnsignedInt("display.window.height");
  const bool fullscreen = config.getBool("display.window.fullscreen");

  flex::RenderWindow window{"example1", width, height, fullscreen, flex::RenderAPI::Vulkan};
  flex::Camera camera;
  flex::VulkanRenderer vulkanRenderer{window, camera};

  flex::Mesh quad{{0, 1, 2, 2, 3, 0},
                  {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                   {{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                   {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                   {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}}};

  flex::Mesh triangle{{0, 1, 2},
                      {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                       {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                       {{-1.f, 0.0f}, {0.0f, 0.0f, 1.0f}}}};

  while (!window.shouldExit()) {
    Time::tick();
    window.update();
    camera.update();

    if (vulkanRenderer.acquireNextFrame()) {
      vulkanRenderer.startDraw();
      vulkanRenderer.drawMesh(quad);
      if (flex::InputHandler::keyPressed("Space")) {
        vulkanRenderer.drawMesh(triangle);
      }
      vulkanRenderer.endDraw();
      vulkanRenderer.present();
    }
  }

  vulkanRenderer.stop();

  return EXIT_SUCCESS;
}
