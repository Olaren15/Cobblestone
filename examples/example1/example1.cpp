#include <FlexEngine.hpp>

int flexMain() {
  flex::RenderWindow window{"example1", 1280, 720, false, flex::RenderAPI::Vulkan};
  flex::VulkanRenderer vulkanRenderer{window};

  flex::Scene scene;
  flex::Mesh cube{{
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
  scene.meshes.push_back(cube);

  vulkanRenderer.loadScene(scene);

  while (!window.shouldExit()) {
    flex::Time::tick();
    window.update();
    scene.update();

    vulkanRenderer.drawScene();
  }

  vulkanRenderer.unloadScene();

  return EXIT_SUCCESS;
}
