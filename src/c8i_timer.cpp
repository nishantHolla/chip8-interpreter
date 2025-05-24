#include "c8i_timer.h"

/////////////////////////////////////////////////
//
// Timer
//
/////////////////////////////////////////////////

/*
 * Construct timer for the interpreter.
 */
C8I_Timer::C8I_Timer(C8I_Memory& memory) :
  memory(memory) {
}

/*
 * Perform timer actions of one frame.
 * This involves decrementing the value of the two timers in memory.
 * Returns true if it did any work, else returns false.
 */
bool C8I_Timer::tick() {
  bool did_work = false;

  // Delay timer
  if (C8I_MEMORY_ACCESS(memory, time_seg, 0) > 0) {
    did_work = true;
    C8I_MEMORY_ACCESS(memory, time_seg, 0) -= 1;
  }

  // Sound timer
  if (C8I_MEMORY_ACCESS(memory, time_seg, 1) > 0) {
    did_work = true;
    C8I_MEMORY_ACCESS(memory, time_seg, 1) -= 1;
  }

  return did_work;
}
