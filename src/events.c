#include <common.h>
#include <ppg.h>

bool ppg_poll_ev() {
  SDL_Event e;
  while (SDL_PollEvent(&e)){
    switch (e.type) {
      case SDL_QUIT: return true; /* If user closes the window */
      case SDL_KEYDOWN: return true; /* If user presses any key */
      case SDL_MOUSEBUTTONDOWN: return true; /* If user clicks the mouse */
      default: return false;
    }
  }
  return false;
}
