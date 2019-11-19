#include <common.h>
#include <ppg.h>

bool ppg_poll_ev(SDL_Event *e, int *key_press) {
  SDL_PollEvent(e);
  switch (e->type) {
    case SDL_QUIT: return true; /* If user closes the window */
    case SDL_KEYDOWN: /* Use number input to select which clip should be drawn */
      switch (e->key.keysym.sym) {
        case SDLK_s: *key_press = 0; break;
        case SDLK_w: *key_press = 1; break;
        case SDLK_a: *key_press = 2; break;
        case SDLK_d: *key_press = 3; break;
        case SDLK_DOWN: *key_press = 4; break;
        case SDLK_UP: *key_press = 5; break;
        case SDLK_ESCAPE: return true;
        default: return false;
      }
      return false;
    case SDL_KEYUP: /* Use to check if user released key */
      *key_press = KEY_RELEASED;
      break;
    case SDL_MOUSEBUTTONDOWN: return true; /* If user clicks the mouse */
    default: return false;
  }
  return false;
}
