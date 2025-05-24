#include "c8i_cpu.h"

/////////////////////////////////////////////////
//
// Instruction
//
/////////////////////////////////////////////////

C8I_Instruction::C8I_Instruction(uint16_t* inst) :
  n0 ( ((*inst) & 0x000f) >>  0 ),
  n1 ( ((*inst) & 0x00f0) >>  4 ),
  n2 ( ((*inst) & 0x0f00) >>  8 ),
  n3 ( ((*inst) & 0xf000) >> 12 ),
  nl ( ((*inst) & 0x00ff) >>  0 ),
  nu ( ((*inst) & 0xff00) >>  8 ),
  n  (*inst) {
}

/////////////////////////////////////////////////
//
// CPU
//
/////////////////////////////////////////////////

/*
 * Construct the CPU with given pointer to the shared memory.
 */
C8I_Cpu::C8I_Cpu(std::shared_ptr<C8I_Memory> memory) :
  memory(memory),
  pc(memory->code_seg.base),
  I(0),
  call_stack{0},
  register_set{0} {
}

/*
 * Performs fetch, decode and execute of one instruction which denotes one cpu tick.
 */
bool C8I_Cpu::tick() {

  // Fetch
  uint16_t* ib = (uint16_t*)&(C8I_MEMORY_ACCESS_RAW(memory, pc));

  // Decode
  C8I_Instruction i(ib);
  debug(i);

  // Execute
  // TODO: Implement all instruction exection

  return true;
}

/*
 * Print the given instruction to stdout
 */
void C8I_Cpu::debug(const C8I_Instruction& instruction) {
  printf("%x %x %x %x = %02x %02x = %02x\n",
         instruction.n3,
         instruction.n2,
         instruction.n1,
         instruction.n0,
         instruction.nu,
         instruction.nl,
         instruction.n);
}
