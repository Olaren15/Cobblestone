#include <FlexEngine.hpp>

int flexMain() {
  flex::RenderWindow window{"example2", 1280, 720, false, flex::RenderAPI::OpenGL};
  flex::OpenGLRenderer renderer{};

  while (!window.shouldExit()) {
    window.update();
    renderer.draw();
  }

  return EXIT_SUCCESS;
}