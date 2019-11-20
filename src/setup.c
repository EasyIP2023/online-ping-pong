#include <common.h>
#include <ppg.h>

static void init_texture_data(ppg *game, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
    game->texture[i].name = '\0';
    game->texture[i].tex = NULL;
  }
}

void ppg_reset_values(ppg *game) {
  game->win = NULL;
  game->ren = NULL;
  game->surf = NULL;
  game->font = NULL;
  game->tsize = 0;
  game->texture = NULL;
  game->player.y_vel = 0;
  game->player.point = 0;
  game->player.box.x = 0;
  game->player.box.y = 0;
  game->player.box.h = 0;
  game->player.box.w = 0;
  game->ball.y_vel = 0;
  game->ball.x_vel = 0;
  game->ball.box.x = 0;
  game->ball.box.y = 0;
  game->ball.box.h = 0;
  game->ball.box.w = 0;
}

void ppg_freeup_game(ppg *game) {
  if (game->texture) {
    for (uint32_t i = 0; i < game->tsize; i++) {
      if (game->texture[i].tex) {
        SDL_DestroyTexture(game->texture[i].tex);
        game->texture[i].tex = NULL;
      }
    }
    FREE(game->texture);
  }
  if (game->font)
    TTF_CloseFont(game->font);
  if (game->surf)
    SDL_FreeSurface(game->surf);
  if (game->ren)
    SDL_DestroyRenderer(game->ren);
  if (game->win)
    SDL_DestroyWindow(game->win);
  IMG_Quit();
  SDL_Quit();
  ppg_reset_values(game);
}

bool ppg_otba(ppg *game, uint32_t size, otba_types type) {
  switch (type) {
    case PPG_TEXTURE:
      game->tsize = size;
      game->texture = (struct _texture *) calloc(sizeof(struct _texture), size * sizeof(struct _texture));
      if (game->texture) { init_texture_data(game, size); return false; }
      else { ppg_log_me(PPG_DANGER, "[x] calloc failed"); return false; }
      return true;
    default:
      ppg_log_me(PPG_DANGER, "Type not defined");
      return false;
  }
}
