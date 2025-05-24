#ifndef C8I_CPU_H_
#define C8I_CPU_H_

#include "c8i_memory.h"
#include "c8i_io.h"

#define C8I_MAX_CALL_STACK_SIZE 1000
#define C8I_REGISTER_SET_SIZE 16

struct C8I_Instruction {
  const uint8_t n0, n1, n2, n3, nl, nu;
  const uint16_t n;

  C8I_Instruction(uint16_t* inst);
};

class C8I_Cpu {
public:
  C8I_Cpu(C8I_Memory& memory);
  bool tick();
  void debug(const C8I_Instruction& instruction);

private:
  C8I_Memory& memory;
  uint16_t pc;
  uint16_t I;
  uint16_t call_stack[C8I_MAX_CALL_STACK_SIZE];
  uint8_t register_set[C8I_REGISTER_SET_SIZE];
};

#endif // C8I_CPU_H_
