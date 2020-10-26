#include <FlexEngine.hpp>

int flexMain() {
  const flex::Configuration config;

  const unsigned int width = config.getUnsignedInt("display.window.width");
  const unsigned int height = config.getUnsignedInt("display.window.height");
  const bool fullscreen = config.getBool("display.window.fullscreen");

  flex::RenderWindow window{"example1", width, height, fullscreen, flex::RenderAPI::Vulkan};
  flex::Camera camera{};
  flex::VulkanRenderer vulkanRenderer{window, camera};

  flex::Mesh quad{{
                      0, 1, 2, 2, 3, 0, // front
                      4, 5, 1, 1, 0, 4, // top
                      5, 4, 7, 7, 6, 5, // back
                      3, 2, 6, 6, 7, 3, // bottom
                      1, 5, 6, 6, 2, 1, // right
                      4, 0, 3, 3, 7, 4  // left
                  },
                  {
                      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},  // 0
                      {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},   // 1
                      {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},    // 2
                      {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},   // 3
                      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}, // 4
                      {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},  // 5
                      {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},   // 6
                      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},  // 7
                  }};

  while (!window.shouldExit()) {
    flex::Time::tick();
    window.update();
    camera.update();

    if (vulkanRenderer.acquireNextFrame()) {
      vulkanRenderer.startDraw();
      vulkanRenderer.drawMesh(quad);
      vulkanRenderer.endDraw();
      vulkanRenderer.present();
    }
  }

  vulkanRenderer.stop();

  return EXIT_SUCCESS;
}
