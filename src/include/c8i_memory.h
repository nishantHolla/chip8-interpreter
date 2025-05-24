#ifndef C8I_MEMORY_H_
#define C8I_MEMORY_H_

#include <fstream>
#include <cstdint>
#include <memory>

#define C8I_MEMORY_SIZE 4096
#define C8I_MEMORY_ACCESS(x, y, z) x[x.y.base + z]

struct C8I_MemorySegment {
  size_t base;
  size_t limit;

  C8I_MemorySegment(size_t base, size_t limit);
};

class C8I_Memory {
public:
  const C8I_MemorySegment font_seg;
  const C8I_MemorySegment screen_seg;
  const C8I_MemorySegment key_seg;
  const C8I_MemorySegment time_seg;
  const C8I_MemorySegment code_seg;

  C8I_Memory(std::istream& stream);
  uint8_t operator[](int i) const;
  uint8_t& operator[](int i);

private:
  std::shared_ptr<uint8_t[]> mm;
};

#endif // !C8I_MEMORY_H_
