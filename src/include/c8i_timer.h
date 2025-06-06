#ifndef C8I_TIMER_H_
#define C8I_TIMER_H_

#include <memory>

#include "c8i_memory.h"

class C8I_Timer {
public:
  C8I_Timer(C8I_Memory& memory);
  bool tick();

private:
  C8I_Memory memory;
};

#endif // !C8I_TIMER_H_
