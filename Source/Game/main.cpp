#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define SDL_MAIN_HANDLED

#include "Graphics/Engine/Engine.hpp"

void setupScene(cbl::gfx::Engine &rendererEngine, cbl::World &scene) {}

int main() {
  cbl::gfx::Engine renderEngine{};

  cbl::gfx::Mesh cube{{
                          0,  1,  3,  3,  2,  0,  // front
                          4,  5,  7,  7,  6,  4,  // right
                          8,  9,  11, 11, 10, 8,  // back
                          12, 13, 15, 15, 14, 12, // right
                          16, 17, 19, 19, 18, 16, // top
                          20, 21, 23, 23, 22, 20, // bottom
                      },
                      {
                          // front
                          {{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}, // 0
                          {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}, // 1
                          {{0.0f, 0.0f, 1.0f}, {0.0, 1.0f, 0.0f}},  // 2
                          {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}, // 3

                          // right
                          {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}, // 4
                          {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // 5
                          {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // 6
                          {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}, // 7

                          // back
                          {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // 8
                          {{0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // 9
                          {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 10
                          {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}, // 11

                          // right
                          {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // 12
                          {{0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}, // 13
                          {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 14
                          {{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}, // 15

                          // top
                          {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 16
                          {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}}, // 17
                          {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}, // 18
                          {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}, // 19

                          // bottom
                          {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 2.0f}}, // 20
                          {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 2.0f}}, // 21
                          {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 2.0f}}, // 22
                          {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 2.0f}}, // 23
                      }};

  cbl::World world;
  world.meshes.push_back(cube);
  renderEngine.loadWorld(world);

  renderEngine.run();

  renderEngine.unloadWorld();

  return EXIT_SUCCESS;
}
