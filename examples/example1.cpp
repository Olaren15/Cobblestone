#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <flexEngine.hpp>

int main() {

  flex::Configuration config;

  int width = config.getInt("display.window.width");

  int height = config.getInt("display.window.height");

  bool fullscreen = config.getBool("display.window.fullscreen");

  std::cout << "Width : " << width << std::endl
            << "Height : " << height << std::endl
            << "Fullscreen : " << (fullscreen ? "true" : "false") << std::endl;

  flex::Window window("window", width, height, fullscreen);

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  return 0;
}