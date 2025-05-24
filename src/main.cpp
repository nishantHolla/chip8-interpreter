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

  SDL_Window* window = SDL_CreateWindow("SDL Test", 640, 280, 0);
  if (!window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    return 2;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    return 3;
  }


  std::ifstream f("1-chip8-logo.ch8", std::ios::binary);
  std::shared_ptr memory = std::make_shared<C8I_Memory>(f);
  C8I_Io io(memory);

  using namespace std::chrono;
  const int fps = 60;
  const milliseconds frame_duration(1000 / fps); // ~16ms

  while (running) {
    auto start = steady_clock::now();

    io.key.tick();
    printf("[ ");
    for (size_t i = 0; i < memory->key_seg.limit; i++) {
      printf("%d ", (int)C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, i));
    }
    printf("]\n");

    SDL_RenderPresent(renderer);

    auto end = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);

    if (elapsed < frame_duration) {
      std::this_thread::sleep_for(frame_duration - elapsed);
    }
  }

  return 0;

  SDL_Quit();
  return 0;
}
