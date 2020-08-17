#include <iostream>

#include <flexEngine.hpp>

#ifdef _WIN32
#include <Windows.h>
int wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
#else
int main() {
#endif // _WIN32

  flex::Configuration config;

  unsigned int width = config.getUnsignedInt("display.window.width");
  unsigned int height = config.getUnsignedInt("display.window.height");
  bool fullscreen = config.getBool("display.window.fullscreen");

  flex::Window window{"window", width, height, fullscreen};
  flex::VulkanRenderer vulkanRenderer{window};

  while (!window.shouldExit()) {
    window.update();

    if (flex::InputHandler::keyPressed("Space")) {
      std::cout << "pew pew !" << std::endl;
    }
  }

  return 0;
}