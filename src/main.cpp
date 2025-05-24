#include <iostream>
#include <fstream>

#include "c8i_memory.h"

int main(void) {
  std::ifstream f("./1-chip8-logo.ch8", std::ios::binary);
  C8I_Memory mem(f);

  for (size_t i = 0; i < mem.font_seg.limit; i++) {
    C8I_MEMORY_ACCESS(mem, font_seg, i) = 1;
  }

  for (size_t i = 0; i < mem.screen_seg.limit; i++) {
    C8I_MEMORY_ACCESS(mem, screen_seg, i) = 2;
  }

  for (size_t i = 0; i < mem.key_seg.limit; i++) {
    C8I_MEMORY_ACCESS(mem, key_seg, i) = 3;
  }

  for (size_t i = 0; i < mem.time_seg.limit; i++) {
    C8I_MEMORY_ACCESS(mem, time_seg, i) = 4;
  }

  for (size_t i = 0; i < C8I_MEMORY_SIZE; i++) {
    printf("%02x ", mem[i]);
  }
  return 0;
}
