#include <vector>

#include <FlexEngine.hpp>

#include "shaders/BlockShaderInfo.hpp"

void setupScene(flex::RendererEngine &rendererEngine, flex::Scene &scene) {
  std::vector<flex::ShaderInformation *> shadersInfo{};

  BlockShaderInfo blockShaderInfo{};
  shadersInfo.push_back(&blockShaderInfo);

  flex::Mesh cube{{
                      0, 1, 2, 2, 3, 0, // front
                                        // 4, 5, 1, 1, 0, 4, // top
                                        // 5, 4, 7, 7, 6, 5, // back
                                        // 3, 2, 6, 6, 7, 3, // bottom
                                        // 1, 5, 6, 6, 2, 1, // right
                                        // 4, 0, 3, 3, 7, 4  // left
                  },
                  {
                      {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // 0
                      {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 1
                      {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},   // 2
                      {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},  // 3
                      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {}},          // 4
                      {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {}},           // 5
                      {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}, {}},            // 6
                      {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {}},           // 7
                  }};
  scene.meshes.push_back(cube);

  rendererEngine.loadScene(scene, shadersInfo);
}

int main() {

  flex::RendererEngine renderEngine{};

  flex::Scene scene;
  setupScene(renderEngine, scene);

  while (renderEngine.isRunning()) {
    renderEngine.update();
  }

  renderEngine.unloadScene();

  return EXIT_SUCCESS;
}
