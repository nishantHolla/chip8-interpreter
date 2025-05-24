#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <thread>
#include "C8I_SDL3/SDL.h"

#include "c8i_memory.h"
#include "c8i_io.h"

bool running = true;

int main(void) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    std::clog << "[ERRR]: SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }

  std::ifstream f("1-chip8-logo.ch8", std::ios::binary);
  std::shared_ptr memory = std::make_shared<C8I_Memory>(f);
  C8I_Io io(memory);

  for (size_t i = 0; i < memory->screen_seg.limit; i++) {
    C8I_MEMORY_ACCESS_PTR(memory.get(), screen_seg, i) = 0xf;
  }

  using namespace std::chrono;
  const int fps = 60;
  const milliseconds frame_duration(1000 / fps); // ~16ms

  while (running) {
    auto start = steady_clock::now();

    io.key.tick();
    if (io.screen.tick()) {
      io.screen.update();
    }

    auto end = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);

    if (elapsed < frame_duration) {
      std::this_thread::sleep_for(frame_duration - elapsed);
    }
  }

  SDL_Quit();
  return 0;
}
