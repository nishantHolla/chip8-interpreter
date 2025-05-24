#include <stdexcept>
#include "c8i_io.h"
#include "C8I_SDL3/SDL.h"

extern bool running;

/////////////////////////////////////////////////
//
// Keyboard
//
/////////////////////////////////////////////////

/*
 * Construct the keyboard device with given reference to the shared memory.
 */
C8I_Keyboard::C8I_Keyboard(C8I_Memory& memory) :
  memory(memory) {
}

/*
 * Perform keyboard actions of one frame.
 * Returns true if it did any work in the tick else returns false.
 */
bool C8I_Keyboard::tick(SDL_Scancode code, uint8_t set_value) {
  uint8_t* access_memory;
  switch (code) {
      case SDLK_x: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  0)); break;
      case SDLK_1: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  1)); break;
      case SDLK_2: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  2)); break;
      case SDLK_3: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  3)); break;
      case SDLK_q: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  4)); break;
      case SDLK_w: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  5)); break;
      case SDLK_e: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  6)); break;
      case SDLK_a: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  7)); break;
      case SDLK_s: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  8)); break;
      case SDLK_d: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg,  9)); break;
      case SDLK_z: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg, 10)); break;
      case SDLK_c: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg, 11)); break;
      case SDLK_4: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg, 12)); break;
      case SDLK_r: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg, 13)); break;
      case SDLK_f: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg, 14)); break;
      case SDLK_v: access_memory = &(C8I_MEMORY_ACCESS(memory, key_seg, 15)); break;
      default:     access_memory = nullptr;
  }

  if (access_memory) {
    *access_memory = set_value;
  }

  return (access_memory != nullptr);
}

/*
 * Print the current state of all keys from the shared memory to stdout.
 */
void C8I_Keyboard::debug() {
  printf("[ ");
  for (size_t i = 0; i < memory.key_seg.limit; i++) {
    printf("%d ", (int)C8I_MEMORY_ACCESS(memory, key_seg, i));
  }
  printf("]\n");
}

/////////////////////////////////////////////////
//
// Screen
//
/////////////////////////////////////////////////

/*
 * Construct the screen device with the given reference to shared memory.
 */
C8I_Screen::C8I_Screen(C8I_Memory& memory) :
  memory(memory) {

  // Create SDL Window
  window = SDL_CreateWindow(
    "Chip 8 Interpreter",
    C8I_SCALED_SCREEN_WIDTH,
    C8I_SCALED_SCREEN_HEIGHT,
    0);
  if (!window) {
    throw std::runtime_error("[ERRR]: Failed to create SDL window: " + std::string(SDL_GetError()));
  }

  // Create SDL Renderer
  renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    throw std::runtime_error("[ERRR]: Failed to create SDL renderer: " + std::string(SDL_GetError()));
  }

  // Create SDL Texture that represents the entire screen.
  screen_texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ABGR8888,
    SDL_TEXTUREACCESS_STREAMING,
    C8I_SCREEN_WIDTH,
    C8I_SCREEN_HEIGHT
  );
  SDL_SetTextureScaleMode(screen_texture, SDL_SCALEMODE_NEAREST);

  // Define SDL Rect of the entire screen.
  screen_rect = {0.0f, 0.0f, C8I_SCALED_SCREEN_WIDTH, C8I_SCALED_SCREEN_HEIGHT};
}

/*
 * Perform screen actions of one frame.
 * This involves reading the screen segment of memory and updating the texture values.
 * Returns true if it did any work in the tick else returns false.
 */
bool C8I_Screen::tick() {
  bool did_work = false;

  uint32_t* pixels;
  int pitch;
  SDL_LockTexture(screen_texture, NULL, (void**)&pixels, &pitch);

  int pixel_count = 0;
  for (size_t i = 0; i < memory.screen_seg.limit; i++) {
    uint8_t screen_pixel_cluster = C8I_MEMORY_ACCESS(memory, screen_seg, i);

    for (int j = 7; j > -1; j--) {
      uint32_t* texture_pixel = &(pixels[pixel_count + (7 - j)]);
      uint8_t screen_pixel = (screen_pixel_cluster >> j) & 1;
      uint32_t expcected_value = screen_pixel ? 0xffffffff : 0x000000ff;
      if (*texture_pixel != expcected_value) {
        did_work = true;
        *texture_pixel = expcected_value;
      }
    }

    pixel_count += 8;
  }

  SDL_UnlockTexture(screen_texture);

  return did_work;
}

/*
 * Update the window with current screen texture.
 */
void C8I_Screen::update() {
  SDL_RenderClear(renderer);
  SDL_RenderTexture(renderer, screen_texture, NULL, &screen_rect);
  SDL_RenderPresent(renderer);
}

/*
 * Free screen resources
 */
C8I_Screen::~C8I_Screen() {
  SDL_DestroyTexture(screen_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

/////////////////////////////////////////////////
//
// Speaker
//
/////////////////////////////////////////////////

/*
 * Construct the speaker device with the given reference to shared memory
 */
C8I_Speaker::C8I_Speaker(C8I_Memory& memory) :
  memory(memory),
  spec{0, 0, 0} {
  spec.channels = 1;
  spec.format = SDL_AUDIO_F32;
  spec.freq = C8I_AUDIO_SAMPLE_RATE;
  stream = SDL_OpenAudioDeviceStream(0xFFFFFFFFu, &spec, NULL, NULL);
  current_sine_sample = 0;
}

/*
 * Perform speaker actions of one frame.
 * This involves checking the timer to see if audio needs to be played or not.
 * Returns true if it did any work in the tick else returns false.
 */
bool C8I_Speaker::tick() {
  check();

  bool did_work = false;

  uint8_t sound_timer_value = C8I_MEMORY_ACCESS(memory, time_seg, 1);
  bool is_playing = isPlaying();

  if (sound_timer_value > 0 && !is_playing) {
    did_work = true;
    play();
  }
  else if (sound_timer_value == 0 && is_playing) {
    did_work = true;
    pause();
  }

  return did_work;
}

/*
 * Start playing the sine wave
 */
void C8I_Speaker::play() {
  SDL_ResumeAudioStreamDevice(stream);
}

/*
 * Stop playing the sine wave
 */
void C8I_Speaker::pause() {
  SDL_PauseAudioStreamDevice(stream);
}

/*
 * Check if the speaker is playing.
 * Returns true if it is playing else returns false.
 */
bool C8I_Speaker::isPlaying() {
  SDL_AudioDeviceID id = SDL_GetAudioStreamDevice(stream);
  return !SDL_AudioDevicePaused(id);
}

/*
 * Toggle the speaker
 */
void C8I_Speaker::toggle() {
  isPlaying() ? pause() : play();
}

/*
 * Check if the audio stream is about to get empty. If yes, then produce more samples of the sine wave.
 */
void C8I_Speaker::check() {
  // check if audio stream is more than half empty
  const int minimum_audio = (C8I_AUDIO_SAMPLE_RATE * sizeof (float)) / 2;
  if (SDL_GetAudioStreamQueued(stream) < minimum_audio) {
    // allocate 512 samples
    static float samples[1024];
    size_t i;

    // produce the samples from the sine wave
    for (i = 0; i < SDL_arraysize(samples); i++) {
      const int freq = C8I_AUDIO_FREQUENCY;
      const float phase = current_sine_sample * freq / (float) C8I_AUDIO_SAMPLE_RATE;
      samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
      current_sine_sample++;
    }
    current_sine_sample %= C8I_AUDIO_SAMPLE_RATE;

    // push the samples to the stream
    SDL_PutAudioStreamData(stream, samples, sizeof (samples));
  }
}

/////////////////////////////////////////////////
//
// IO
//
/////////////////////////////////////////////////

/*
 * Initialize the IO of the interpreter with an instance of keyboard, screen and speaker with the
 * given reference to shared memory.
 */
C8I_Io::C8I_Io(C8I_Memory& memory) :
  key(memory),
  screen(memory),
  speaker(memory) {
}

/*
 * Perform all IO actions of one frame.
 * This involves polling for inputs, performing tick operations of keyboard, screen and speaker.
 * Returns true if it did any work in the tick else returns false.
 */
bool C8I_Io::tick() {
  // Keyboard
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // If quit, return with no work done
    if (event.type == SDL_EVENT_QUIT) {
      return false;
    }

    // If key press or release, call tick function of keyboard
    else if (event.type == SDL_EVENT_KEY_UP) {
      key.tick(event.key.keysym.scancode, 0);
    }
    else if (event.type == SDL_EVENT_KEY_DOWN) {
      key.tick(event.key.keysym.scancode, 1);
    }
  }

  // Screen
  screen.tick();

  // Speaker
  speaker.tick();

  return true;
}
