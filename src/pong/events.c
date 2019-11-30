#include <common.h>
#include <ppg.h>

bool ppg_poll_ev(SDL_Event *e, uint32_t *key) {
  SDL_PollEvent(e);
  switch (e->type) {
    case SDL_QUIT: return true; /* If user closes the window */
    case SDL_KEYDOWN: /* Use number input to select which clip should be drawn */
      switch (e->key.keysym.sym) {
        case SDLK_s: *key = 0; break;
        case SDLK_w: *key = 1; break;
        case SDLK_a: *key = 2; break;
        case SDLK_d: *key = 3; break;
        case SDLK_DOWN: *key = 4; break;
        case SDLK_UP: *key = 5; break;
        case SDLK_ESCAPE: *key = RET_TO_MENU; break;
        default: return false;
      }
      return false;
    case SDL_KEYUP: /* Use to check if user released key */
      *key = KEY_RELEASED;
      break;
    case SDL_MOUSEBUTTONDOWN: /* If user clicks the mouse */
      *key = MOUSE_BUTTON_PRESSED;
      break;
    default: return false;
  }
  return false;
}
