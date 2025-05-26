#ifndef C8I_IO_H_
#define C8I_IO_H_

#define C8I_SCREEN_WIDTH 64
#define C8I_SCREEN_HEIGHT 32
#define C8I_SCREEN_BYTES_PER_ROW (C8I_SCREEN_WIDTH/8)
#define C8I_SCREEN_SCALE 16
#define C8I_SCALED_SCREEN_WIDTH (C8I_SCREEN_WIDTH * C8I_SCREEN_SCALE)
#define C8I_SCALED_SCREEN_HEIGHT (C8I_SCREEN_HEIGHT * C8I_SCREEN_SCALE)
#define C8I_AUDIO_SAMPLE_RATE 8000
#define C8I_AUDIO_FREQUENCY 300

#include <SDL3/SDL.h>
#include "c8i_memory.h"

class C8I_Keyboard {
public:
  C8I_Keyboard(C8I_Memory& memory);
  bool tick(SDL_Keycode code, uint8_t set_value);
  void debug();

private:
  C8I_Memory& memory;

};

class C8I_Screen {
public:
  C8I_Screen(C8I_Memory& memory);
  ~C8I_Screen();
  bool tick();
  void update();

private:
  C8I_Memory& memory;
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* screen_texture;
  SDL_FRect screen_rect;
};

class C8I_Speaker {
public:
  C8I_Speaker(C8I_Memory& memory);
  bool tick();

private:
  C8I_Memory& memory;
  SDL_AudioSpec spec;
  SDL_AudioStream* stream;
  int current_sine_sample;

  void play();
  void pause();
  bool isPlaying();
  void toggle();
  void check();
};

class C8I_Io {
public:
  C8I_Keyboard key;
  C8I_Screen screen;
  C8I_Speaker speaker;

  C8I_Io(C8I_Memory& memory);
  bool tick();
};

#endif // !C8I_IO_H_
