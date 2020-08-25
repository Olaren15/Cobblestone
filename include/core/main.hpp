// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
#pragma once

#include <iostream>

#define SDL_MAIN_HANDLED
#if defined _WIN32 && !defined NDEBUG
#include <Windows.h>
#endif

int flexMain();

int main() {
  int returnValue;

  try {
#if defined _WIN32 && !defined NDEBUG
    // Show console output in debug mode
    AllocConsole();

    FILE *fDummy;
    // ReSharper disable StringLiteralTypo
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    // ReSharper restore StringLiteralTypo

    returnValue = flexMain();

#endif
  } catch (...) {
    return EXIT_FAILURE;
  }

  return returnValue;
}
