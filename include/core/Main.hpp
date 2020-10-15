// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
#pragma once

#define SDL_MAIN_HANDLED
#if defined _WIN32 && !defined NDEBUG && !defined __MINGW32__
#include <Windows.h>
#endif

int flexMain();

int main() {
  int returnValue;

  try {
#if defined _WIN32 && !defined NDEBUG && !defined __MINGW32__
    // Show console output in debug mode
    AllocConsole();

    FILE *fDummy;
    // ReSharper disable StringLiteralTypo
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    // ReSharper restore StringLiteralTypo
#endif

    returnValue = flexMain();

  } catch (std::exception &ex) {
    std::cout << ex.what() << '\n';
    return EXIT_FAILURE;
  }

  return returnValue;
}
