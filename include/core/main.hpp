// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
#pragma once

#define SDL_MAIN_HANDLED

int flexMain();

#if defined _WIN32 && defined NDEBUG
#include <Windows.h>
// ReSharper disable once CppInconsistentNaming
int wWinMain([[maybe_unused]] _In_ HINSTANCE hInstance, [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
             [[maybe_unused]] _In_ LPWSTR lpCmdLine, [[maybe_unused]] _In_ int nShowCmd) {
#else
int main() {
#endif // _WIN32
  return flexMain();
}
