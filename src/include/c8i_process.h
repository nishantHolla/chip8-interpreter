#ifndef C8I_PROCESS_H_
#define C8I_PROCESS_H_

#include <string>

#include "c8i_memory.h"
#include "c8i_timer.h"
#include "c8i_io.h"
#include "c8i_cpu.h"

class C8I_Process {
public:
  C8I_Process(const std::string& rom_file_path);
  void init_memory_for_testing();
  void execute();

private:
  std::ifstream stream;
  std::shared_ptr<C8I_Memory> memory;
  C8I_Timer timer;
  C8I_Io io;
  C8I_Cpu cpu;

  std::ifstream init_stream(const std::string& rom_file_path);
};

#endif // C8I_PROCESS_H_
