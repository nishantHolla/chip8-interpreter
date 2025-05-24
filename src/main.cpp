#include <iostream>
#include <cstdlib>
#include <ctime>

#include "C8I_SDL3/SDL.h"
#include "c8i_process.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./c8i <rom.ch8>";
    return 1;
  }

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    std::cerr << "[ERRR]: SDL_Init failed: " << SDL_GetError() << "\n";
    return 2;
  }

  std::srand(std::time(0));

  C8I_Process process(argv[1]);
  // process.init_memory_for_testing();
  process.execute();

  SDL_Quit();
  return 0;
}
