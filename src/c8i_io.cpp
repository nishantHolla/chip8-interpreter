#include "c8i_io.h"
#include "C8I_SDL3/SDL.h"

extern bool running;

C8I_Keyboard::C8I_Keyboard(std::shared_ptr<C8I_Memory> memory) :
  memory(memory) {
}

void C8I_Keyboard::tick() {
  // check for keyboard inputs

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      running = false;
    }

    else if (event.type == SDL_EVENT_KEY_DOWN) {
      switch (event.key.keysym.scancode) {
        case SDLK_x: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  0) = 1; break;
        case SDLK_1: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  1) = 1; break;
        case SDLK_2: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  2) = 1; break;
        case SDLK_3: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  3) = 1; break;
        case SDLK_q: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  4) = 1; break;
        case SDLK_w: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  5) = 1; break;
        case SDLK_e: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  6) = 1; break;
        case SDLK_a: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  7) = 1; break;
        case SDLK_s: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  8) = 1; break;
        case SDLK_d: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  9) = 1; break;
        case SDLK_z: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 10) = 1; break;
        case SDLK_c: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 11) = 1; break;
        case SDLK_4: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 12) = 1; break;
        case SDLK_r: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 13) = 1; break;
        case SDLK_f: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 14) = 1; break;
        case SDLK_v: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 15) = 1; break;

        default:
          break;
      }
    }
    else if (event.type == SDL_EVENT_KEY_UP) {
      switch (event.key.keysym.scancode) {
        case SDLK_x: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  0) = 0; break;
        case SDLK_1: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  1) = 0; break;
        case SDLK_2: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  2) = 0; break;
        case SDLK_3: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  3) = 0; break;
        case SDLK_q: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  4) = 0; break;
        case SDLK_w: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  5) = 0; break;
        case SDLK_e: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  6) = 0; break;
        case SDLK_a: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  7) = 0; break;
        case SDLK_s: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  8) = 0; break;
        case SDLK_d: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg,  9) = 0; break;
        case SDLK_z: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 10) = 0; break;
        case SDLK_c: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 11) = 0; break;
        case SDLK_4: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 12) = 0; break;
        case SDLK_r: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 13) = 0; break;
        case SDLK_f: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 14) = 0; break;
        case SDLK_v: C8I_MEMORY_ACCESS_PTR(memory.get(), key_seg, 15) = 0; break;

        default:
          break;
      }
    }
  }

}

C8I_Io::C8I_Io(std::shared_ptr<C8I_Memory> memory) :
  key(memory) {
}
