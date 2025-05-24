#include "c8i_cpu.h"
#include <iostream>
#include <cstdlib>

#define C8I_EXEC_SUCCESS             0
#define C8I_EXEC_ERR_UNKNOWN_INST    1
#define C8I_EXEC_ERR_STACK_OVERFLOW  2
#define C8I_EXEC_ERR_STACK_UNDERFLOW 3

/////////////////////////////////////////////////
//
// Instructin
//
/////////////////////////////////////////////////

C8I_Instruction::C8I_Instruction(uint8_t ub, uint8_t lb) :
  n0 ( (lb & 0x0f) >> 0 ),
  n1 ( (lb & 0xf0) >> 4 ),
  n2 ( (ub & 0x0f) >> 0 ),
  n3 ( (ub & 0xf0) >> 4 ),
  nl (lb),
  nu (ub),
  n  ((ub << 8) | lb) {
}

/////////////////////////////////////////////////
//
// CPU
//
/////////////////////////////////////////////////

/*
 * Construct the CPU with given pointer to the shared memory.
 */
C8I_Cpu::C8I_Cpu(C8I_Memory& memory) :
  memory(memory),
  pc(memory.code_seg.base),
  I(0),
  call_stack{0},
  call_stack_head(0),
  register_set{0} {
}

/*
 * Performs fetch, decode and execute of one instruction which denotes one cpu tick.
 * If a pointer to bool is passed, it is set to true if it executed a draw instruction
 */
bool C8I_Cpu::tick(bool* has_drawn) {

  // Fetch
  uint8_t ub = C8I_MEMORY_ACCESS_RAW(memory, pc);
  uint8_t lb = C8I_MEMORY_ACCESS_RAW(memory, pc+1);
  pc += 2;

  // Decode
  C8I_Instruction i(ub, lb);
  if (has_drawn) {
    *has_drawn = false;
  }
  // printf("Instruction: ");
  // debug_instruction(i);

  // Execute

  // TODO: 0NNN - Execute machine language subroutine at address NNN

  // 00E0 - Clear the screen
  if (i.n == 0x00E0)  {
    for (size_t i = 0; i < memory.screen_seg.limit; i++) {
      C8I_MEMORY_ACCESS(memory, screen_seg, i) = 0;
    }
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 00EE - Return from subroutine
  if (i.n == 0x00EE) {
#ifdef __C8I_MEM_BOUND_CHECK__
    if (call_stack_head == 0) {
      return execution_callback(i, C8I_EXEC_ERR_STACK_UNDERFLOW);
    }
#endif // __C8I_MEM_BOUND_CHECK__
    pc = call_stack[--call_stack_head];
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 1NNN - Jump to address NNN
  else if (i.n3 == 0x1) {
    pc = (i.n2 << 8) | i.nl;
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 2NNN - Execute subroutine at address NNN
  else if (i.n3 == 0x2) {
#ifdef __C8I_MEM_BOUND_CHECK__
    if (call_stack_head >= C8I_MAX_CALL_STACK_SIZE) {
      return execution_callback(i, C8I_EXEC_ERR_STACK_OVERFLOW);
    }
#endif // __C8I_MEM_BOUND_CHECK__
    call_stack[call_stack_head++] = pc;
    pc = (i.n2 << 8) | i.nl;
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 3XNN - Skip the following instruction if value in register VX is NN
  else if (i.n3 == 0x3) {
    if (register_set[i.n2] == i.nl) {
      pc += 2;
    }
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 4XNN - Skip the following instruction if value in register VX is not equal to NN
  else if (i.n3 == 0x4) {
    if (register_set[i.n2] != i.nl) {
      pc += 2;
    }
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 5XY0 - Skip the following instruction if value in register VX is equal to the value in VY
  else if (i.n3 == 0x5) {
    if (register_set[i.n2] == register_set[i.n1]) {
      pc += 2;
    }
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 6XNN - Store number NN in register VX
  else if (i.n3 == 0x6) {
    register_set[i.n2] = i.nl;
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 7XNN - Add value NN to register VX
  else if (i.n3 == 0x7) {
    register_set[i.n2] += i.nl;
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // 8--- instructions
  if (i.n3 == 0x8) {

    // 8XY0 - Store the value of register VY in register VX
    if (i.n0 == 0x0) {
      register_set[i.n2] = register_set[i.n1];
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // 8XY1 - Set VX = VX OR VY
    else if (i.n0 == 0x1) {
      register_set[i.n2] |= register_set[i.n1];
      register_set[15] = 0;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // 8XY2 - Set VX = VX AND VY
    else if (i.n0 == 0x2) {
      register_set[i.n2] &= register_set[i.n1];
      register_set[15] = 0;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // 8XY3 - Set VX = VX XOR VY
    else if (i.n0 == 0x3) {
      register_set[i.n2] ^= register_set[i.n1];
      register_set[15] = 0;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // 8XY4 - Add value of register VY to register VX
    //        Set VF to 01 if carry occures else set to 0
    else if (i.n0 == 0x4) {
      uint16_t result = register_set[i.n2] + register_set[i.n1];
      register_set[i.n2] = static_cast<uint8_t>(result);
      register_set[15] = result > 255;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // 8XY5 - Subtract value of register VY from register VX
    //        Set VF to 01 if borrow occures else set to 0
    else if (i.n0 == 0x5) {
      bool flag = register_set[i.n2] >= register_set[i.n1];
      register_set[i.n2] -= register_set[i.n1];
      register_set[15] = flag;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // 8XY6 - Store the value of register VY shifted right one bit in register VX
    //        Set register VF to the least significant bit prior to the shift
    else if (i.n0 == 0x6) {
      bool flag = register_set[i.n2] & 1;
      register_set[i.n2] = register_set[i.n1] >> 1;
      register_set[15] = flag;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // 8XY7 - Set register VX to the value of VY - VX.
    //        Set VF to 0 if borrow occures else  set to 1
    else if (i.n0 == 0x7) {
      bool flag = register_set[i.n1] >= register_set[i.n2];
      register_set[i.n2] = register_set[i.n1] - register_set[i.n2];
      register_set[15] = flag;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // 8XYE - Store the value of register VY shifted left one bit in register VX
    //        Set VF to most significant bit prior to the shift
    else if (i.n0 == 0xE) {
      bool flag = register_set[i.n2] >> 7;
      register_set[i.n2] = register_set[i.n1] << 1;
      register_set[15] = flag;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

  }

  // 9XY0 - Skip the following instruction if the value of register VX is not equal
  //        to value of register VY
  else if (i.n3 == 0x9) {
    if (register_set[i.n2] != register_set[i.n1]) {
      pc += 2;
    }
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // ANNN - Store memory address NNN in register I
  else if (i.n3 == 0xA) {
    I = (i.n2 << 8) | i.nl;
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // BNNN - Jump to address NNN + V0
  else if (i.n3 == 0xB) {
    pc = ((i.n2 << 8) + i.nl) + register_set[0];
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // CXNN - Generate random number and mask it with NN
  else if (i.n3 == 0xC) {
    register_set[i.n2] = (rand() % 256) & i.nl;
    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // DXYN - Draw sprite at poisition VX, VY with N bytes of sprite data starting at address stored
  //        in I. Set VF to 01 if any set pixels are changed to unset and 00 otherwise
  else if (i.n3 == 0xD) {
    if (has_drawn) {
      *has_drawn = true;
    }

    uint8_t x = register_set[i.n2];
    uint8_t y = register_set[i.n1];
    uint8_t n = i.n0;

    uint8_t xp = x % C8I_SCREEN_WIDTH;
    uint8_t yp = y % C8I_SCREEN_HEIGHT;

    register_set[15] = 0;

    for (uint8_t row = 0; row < n; row++) {
      uint8_t sprite_byte = C8I_MEMORY_ACCESS_RAW(memory, I + row);

      if (yp + row >= C8I_SCREEN_HEIGHT) {
        continue;
      }

      for (uint8_t col = 0; col < 8; col++) {
        if (xp + col >= C8I_SCREEN_WIDTH) {
          continue;
        }

        uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 1;
        uint8_t screen_x = (xp + col) / 8;
        uint8_t screen_x_padding = (xp + col) % 8;
        uint8_t screen_y = (yp + row) * C8I_SCREEN_BYTES_PER_ROW;
        uint32_t screen_idx = screen_y + screen_x;
        uint8_t* screen_byte = &C8I_MEMORY_ACCESS(memory, screen_seg, screen_idx);
        uint8_t screen_pixel = (*screen_byte >> (7 - screen_x_padding)) & 1;

        if (screen_pixel && sprite_pixel) {
          register_set[15] = 1;
        }

        *screen_byte = *screen_byte ^ (sprite_pixel << (7 - screen_x_padding));
      }
    }

    return execution_callback(i, C8I_EXEC_SUCCESS);
  }

  // E--- instructions
  else if (i.n3 == 0xE) {

    // EX9E - Skip the following instruction if the key corresponding to hex value currently
    //        stored in register VX is pressed
    if (i.nl == 0x9E) {
      uint8_t key = register_set[i.n2];
      if (C8I_MEMORY_ACCESS(memory, key_seg, key) == 1) {
        pc += 2;
      }
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // EXA1 - Skip the following instruction if the key corresponding to the hex value currently
    //        storked in register VX is not pressed
    else if (i.nl == 0xA1) {
      uint8_t key = register_set[i.n3];
      if (C8I_MEMORY_ACCESS(memory, key_seg, key) == 0) {
        pc += 2;
      }
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

  }

  // F--- instructions
  else if (i.n3 == 0xF) {

    // FX07 - Store the current value of delay timer in register VX
    if (i.nl == 0x07) {
      register_set[i.n2] = C8I_MEMORY_ACCESS(memory, time_seg, 0);
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // FX0A - Wait for a keypress and store the result in register VX
    else if (i.nl == 0x0A) {
      for (size_t idx = 0; idx < memory.key_seg.limit; idx++) {
        uint8_t key = C8I_MEMORY_ACCESS(memory, key_seg, idx);
        if (key) {
          register_set[i.n2] = key;
          return execution_callback(i, C8I_EXEC_SUCCESS);
        }
      }
      pc -= 2;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // FX15 - Set the delay timer to the value of register VX
    else if (i.nl == 0x15) {
      C8I_MEMORY_ACCESS(memory, time_seg, 0) = register_set[i.n2];
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // FX18 - Set the sound timer to the value of register VX
    else if (i.nl == 0x18) {
      C8I_MEMORY_ACCESS(memory, time_seg, 1) = register_set[i.n2];
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // FX1E - Add the value stored in register VX to register I
    else if (i.nl == 0x1E) {
      I += register_set[i.n2];
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // FX29 - Set I to the memory address of sprite data correspoinding to the hexadecimal
    //        digit stored in register VX
    else if (i.nl == 0x29) {
      I = memory.font_seg.base + (i.n2 * 5);
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // FX33 - Store the BCD equivalent of the value storend in register VX at address I, I+1 and I+2
    else if (i.nl == 0x33) {
      uint8_t val = register_set[i.n2];

      for (int b = 2; b > -1; b--, val /= 10) {
        int8_t digit = val ? val % 10 : 0;
        C8I_MEMORY_ACCESS_RAW(memory, I+b) = digit;
      }

      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // FX55 - Store the value of register V0 to VX inclusive in memory starting at address I
    //        I is set to I + X + 1 after operation
    else if (i.nl == 0x55) {
      for (int idx = 0; idx <= i.n2; idx++) {
        C8I_MEMORY_ACCESS_RAW(memory, I + idx) = register_set[idx];
      }
      I += i.n2 + 1;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

    // FX65 - Fill registers V0 to VX inclusive with the values stored in memory starting at address I
    //        I is set to I + X + 1 after operation
    else if (i.nl == 0x65) {
      for (int idx = 0; idx <= i.n2; idx++) {
        register_set[idx] = C8I_MEMORY_ACCESS_RAW(memory, I + idx);
      }
      I += i.n2 + 1;
      return execution_callback(i, C8I_EXEC_SUCCESS);
    }

  }

  return execution_callback(i, C8I_EXEC_ERR_UNKNOWN_INST);
  // debug_state();
  // printf("------------------------\n");
}

/*
 * Callback for when execution of an instruction is completed
 * State value tells how the execution of the instruction went
 */
bool C8I_Cpu::execution_callback(const C8I_Instruction& i, int16_t state) {
  char message[100];

  if (state == C8I_EXEC_SUCCESS) {
    return true;
  }
  if (state == C8I_EXEC_ERR_UNKNOWN_INST) {
    sprintf(message, "Unknown instruction: %04x", i.n);
  }
  else if (state == C8I_EXEC_ERR_STACK_OVERFLOW) {
    sprintf(message, "Stack overflow at instruction: %04x", i.n);
  }
  else if (state == C8I_EXEC_ERR_STACK_UNDERFLOW) {
    sprintf(message, "Stack underflow at instruction: %04x", i.n);
  }

  throw std::runtime_error(message);
}

/*
 * Print the given instruction to stdout
 */
void C8I_Cpu::debug_instruction(const C8I_Instruction& instruction) {
  printf("%x %x %x %x = %02x %02x = %04x\n",
         instruction.n3,
         instruction.n2,
         instruction.n1,
         instruction.n0,
         instruction.nu,
         instruction.nl,
         instruction.n);
}

/*
 * Print the state of the cpu to stdout
 */
void C8I_Cpu::debug_state() {
  for (int i = 0; i < 16; i++) {
    printf("V%x = %02x\n", i, register_set[i]);
  }
  printf("pc = %04x\n", pc);
  printf("I  = %04x\n", I);
}
