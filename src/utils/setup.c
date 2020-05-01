#include <common.h>
#include <ppg.h>

static void init_texture_data(ppg *game) {
  SDL_Color color = {0, 0, 0, 0};
  for (uint32_t i = 0; i < game->di_size; i++) {
    game->display_items[i].surf = NULL;
    game->display_items[i].tex = NULL;
    game->display_items[i].font = NULL;
    ppg_copy_sdl_color(game, i, color);
  }
}

static void init_audio_data(ppg *game) {
  for (uint32_t i = 0; i < game->asize; i++) {
    game->audio[i].music = NULL;
    game->audio[i].effect = NULL;
  }
}

void ppg_reset_values(ppg *game) {
  game->win = NULL;
  game->ren = NULL;
  game->di_size = 0;
  game->display_items = NULL;
  game->asize = 0;
  game->audio = NULL;
  game->ball.y_vel = 0;
  game->ball.x_vel = 0;
  game->ball.box.x = 0;
  game->ball.box.y = 0;
  game->ball.box.h = 0;
  game->ball.box.w = 0;
  for (uint32_t i = 0; i < 2; i++) {
    game->player[i].y_vel = 0;
    game->player[i].points = 0;
    game->player[i].box.x = 0;
    game->player[i].box.y = 0;
    game->player[i].box.h = 0;
    game->player[i].box.w = 0;
    game->player[i].terminate = false;
  }
}

void ppg_freeup_game(ppg *game) {

  if (game->display_items) {
    for (uint32_t i = 0; i < game->di_size; i++) {
      if (game->display_items[i].tex) {
        SDL_DestroyTexture(game->display_items[i].tex);
        game->display_items[i].tex = NULL;
      }
      if (game->display_items[i].font) {
        TTF_CloseFont(game->display_items[i].font);
        game->display_items[i].font = NULL;
      }
      if (game->display_items[i].surf) {
        SDL_FreeSurface(game->display_items[i].surf);
        game->display_items[i].surf = NULL;
      }
    }
    FREE(game->display_items);
  }

  if (game->audio) {
    for (uint32_t i = 0; i < game->asize; i++) {
      if (game->audio[i].music) {
        Mix_FreeMusic(game->audio[i].music);
        game->audio[i].music = NULL;
      }
      if (game->audio[i].effect) {
        Mix_FreeChunk(game->audio[i].effect);
        game->audio[i].effect = NULL;
      }
    }
    FREE(game->audio);
  }

  if (game->ren)
    SDL_DestroyRenderer(game->ren);

  if (game->win)
    SDL_DestroyWindow(game->win);

  Mix_CloseAudio();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

bool ppg_otba(ppg *game, uint32_t size, otba_types type) {
  switch (type) {
    case PPG_TEXTURE:
      game->di_size = size;
      game->display_items = (struct _display_item *) calloc(size, sizeof(struct _display_item));
      if (game->display_items) { init_texture_data(game); return false; }
      else { ppg_log_me(PPG_DANGER, "[x] calloc: %s", strerror(errno)); return false; }
      return true;
    case PPG_AUDIO:
      game->asize = size;
      game->audio = (struct _audio *) calloc(size, sizeof(struct _audio));
      if (game->audio) { init_audio_data(game); return false; }
      else { ppg_log_me(PPG_DANGER, "[x] calloc: %s", strerror(errno)); return false; }
      return true;
    default:
      ppg_log_me(PPG_DANGER, "Type not defined");
      return false;
  }
}
