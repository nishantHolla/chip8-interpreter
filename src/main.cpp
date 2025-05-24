#include <iostream>

#include "C8I_SDL3/SDL.h"
#include "c8i_process.h"

int main(void) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    std::clog << "[ERRR]: SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }

  C8I_Process process("1-chip8-logo.ch8");
  process.init_memory_for_testing();
  process.execute();

  SDL_Quit();
  return 0;
}
