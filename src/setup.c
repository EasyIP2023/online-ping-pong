#include <common.h>
#include <ppg.h>

void freeup_ppg(ppg *game) {
  if (game->texture) {
    for (uint32_t i = 0; i < game->tsize; i++) {
      if (game->texture[i].tex) {
        SDL_DestroyTexture(game->texture[i].tex);
        game->texture[i].tex = NULL;
      }
    }
    FREE(game->texture);
  }
  if (game->surf)
    SDL_FreeSurface(game->surf);
  if (game->ren)
    SDL_DestroyRenderer(game->ren);
  if (game->win)
    SDL_DestroyWindow(game->win);
  SDL_Quit();
}

void init_texture_data(ppg *game, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
    game->texture[i].name = '\0';
    game->texture[i].tex = NULL;
  }
}

bool ppg_otba(ppg *game, uint32_t size, otba_types type) {
  switch (type) {
    case PPG_TEXTURE:
      game->texture = (struct texture *) calloc(sizeof(struct texture), size * sizeof(struct texture));
      if (game->texture) { init_texture_data(game, size); return false; }
      else { ppg_log_me(PPG_DANGER, "[x] calloc calloc failed"); return false; }
      return true;
    default:
      ppg_log_me(PPG_DANGER, "Type not defined");
      return false;
  }
}
