#ifndef C8I_IO_H_
#define C8I_IO_H_

#include "c8i_memory.h"

class C8I_Keyboard {
public:
  C8I_Keyboard(std::shared_ptr<C8I_Memory> memory);
  void tick();

private:
  std::shared_ptr<C8I_Memory> memory;

};

class C8I_Io {
public:
  C8I_Keyboard key;

  C8I_Io(std::shared_ptr<C8I_Memory> memory);
};

#endif // !C8I_IO_H_
