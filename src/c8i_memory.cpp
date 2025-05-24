#ifdef __C8I_MEM_BOUND_CHECK__
#include <stdexcept>
#endif // __C8I_MEM_BOUND_CHECK__

#include <sstream>
#include <cstring>

#include "c8i_memory.h"

/////////////////////////////////////////////////
//
// Static Memory Values
//
/////////////////////////////////////////////////

static const uint8_t font_map[] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/////////////////////////////////////////////////
//
// Memory Segment
//
/////////////////////////////////////////////////

/*
 * Construct a memory segment with the base value and the limit.
 */
C8I_MemorySegment::C8I_MemorySegment(size_t base, size_t limit) :
  base(base),
  limit(limit) {
}

/////////////////////////////////////////////////
//
// Memory
//
/////////////////////////////////////////////////

/*
 * Construct memory with the input stream for the ROM that needs to be loaded onto memory.
 */
C8I_Memory::C8I_Memory(std::istream& stream) :
  font_seg(2, 80),
  screen_seg(84, 256),
  key_seg(342, 16),
  time_seg(360, 2),
  code_seg(512, 3584),
  mm(new uint8_t[C8I_MEMORY_SIZE]) {

  // initialize memory to zero
  for (int i = 0; i < C8I_MEMORY_SIZE; i++) {
    mm[i] = 0;
  }

  // load font map
  memcpy(&(mm[font_seg.base]), font_map, font_seg.limit);

  // load code
  uint8_t c;
  size_t i = code_seg.base;
  while (stream.read((char *)(&c), 1)) {
    mm[i++] = c;
  }
}

/*
 * Print the current state of memory to stdout
 */
void C8I_Memory::debug() {
  for (size_t i = 0; i < C8I_MEMORY_SIZE; i++) {
    printf("%02x ", mm[i]);
  }
}

/*
 * Overload [] operator for memory access.
 * Runtime bound checking is done only if the __C8I_MEM_BOUND_CHECK__ is defined.
 */
uint8_t C8I_Memory::operator[](int i) const {
#ifdef __C8I_MEM_BOUND_CHECK__
  if (i < 0 || i > C8I_MEMORY_SIZE) {
    std::stringstream ss;
    ss << "[ERRR]: Memory access " << i << " out of bound\n";
    throw std::runtime_error(ss.str());
  }
#endif

  return mm[i];
}

/*
 * Overload [] operator for memory access.
 * Runtime bound checking is done only if the __C8I_MEM_BOUND_CHECK__ is defined.
 */
uint8_t& C8I_Memory::operator[](int i) {
#ifdef __C8I_MEM_BOUND_CHECK__
  if (i < 0 || i >= C8I_MEMORY_SIZE) {
    std::stringstream ss;
    ss << "[ERRR]: Memory access " << i << " out of bound\n";
    throw std::runtime_error(ss.str());
  }
#endif

  return mm[i];
}
