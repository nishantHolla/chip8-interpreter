#include "c8i_process.h"

#include <stdexcept>
#include <fstream>
#include <chrono>
#include <thread>
#include <iostream>

/////////////////////////////////////////////////
//
// Process
//
/////////////////////////////////////////////////

/*
 * Constructs a CHIP 8 Process by allocaing memory, timer, io and cpu.
 */
C8I_Process::C8I_Process(const std::string& rom_file_path) :
  stream(init_stream(rom_file_path)),
  memory(stream),
  timer(memory),
  io(memory),
  cpu(memory) {
  stream.close();
}

/*
 * Initialize the memory space with some default values.
 * Used for testing only.
 */
void C8I_Process::init_memory_for_testing() {
  // Initialize screen segement with some value
  for (size_t i = 0; i < memory.screen_seg.limit; i++) {
    C8I_MEMORY_ACCESS(memory, screen_seg, i) = 0xf0;
  }

  // Initialize timer segment with some value
  C8I_MEMORY_ACCESS(memory, time_seg, 1) = 60;
}

/*
 * Execute the loaded program
 */
void C8I_Process::execute() {
  using namespace std::chrono;
  const milliseconds frame_duration(1000 / C8I_FPS);
  const int instructions_per_frame = C8I_CPU_HZ / C8I_FPS;

  while (true) {
    auto start = steady_clock::now();

    // io.tick returns false if user wants to quit
    if (!io.tick()) break;

    // run multiple cpu instructions per frame
    bool has_drawn = false;
    for (int i = 0; i < instructions_per_frame; i++) {
      if (has_drawn) {
        break;
      }
      cpu.tick(&has_drawn);
    }

    // tick timer
    timer.tick();

    // update screen
    io.screen.update();

    auto end = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);

    if (elapsed < frame_duration) {
      // std::cout << "[INFO]: Frame buffer time: +" << (frame_duration - elapsed).count() << std::endl;
      std::this_thread::sleep_for(frame_duration - elapsed);
    }
  }

}

/*
 * Initializes and returns an input stream for the reading of the ROM file.
 */
std::ifstream C8I_Process::init_stream(const std::string& rom_file_path) {
  std::ifstream stream(rom_file_path, std::ios::binary);
  if (!stream) {
    throw std::runtime_error("Failed to open " + rom_file_path + " for reading");
  }

  return stream;
}
