#include <common.h>
#include <ppg.h>

static bool bmp(ppg *game, uint32_t cur_tex, const char *path) {
  game->surf = SDL_LoadBMP(path);
  if (game->surf) {
    game->texture[cur_tex].tex = SDL_CreateTextureFromSurface(game->ren, game->surf);
    SDL_FreeSurface(game->surf); game->surf = NULL;
    if (!game->texture[cur_tex].tex) {
      ppg_log_me(PPG_DANGER, "[x] SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
      return false;
    }
    ppg_log_me(PPG_SUCCESS, "SDL Created Texture From Surface");
    return true;
  }
  else {
    ppg_log_me(PPG_DANGER, "[x] Failed to load a BMP");
    return false;
  }
}

static bool img(ppg *game, uint32_t cur_tex, const char *path) {
  if (!game->ren) {
    ppg_log_me(PPG_DANGER, "[x] must make a call to SDL_CreateRenderer()");
    return false;
  }
  game->texture[cur_tex].tex = IMG_LoadTexture(game->ren, path);
  if (!game->texture[cur_tex].tex) {
    ppg_log_me(PPG_DANGER, "[x] IMG_LoadTexture failed, Error: %s", SDL_GetError());
    return false;
  }
  return true;
}

bool ppg_load_texture(ppg *game, uint32_t cur_tex, const char *path, texture_types type) {
  switch (type) {
    case PPG_BMP_TEX:
      return !bmp(game, cur_tex, path);
    case PPG_IMG_TEX:
      return !img(game, cur_tex, path);
    default:
      ppg_log_me(PPG_DANGER, "[x] failed to pass correct enum member");
      return true;
  }
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y.
* Whilst preserving the texture's width and height. Taking a clip
* of the texture and if clip has and address it's pointing to
* use clip's width and height instead of textures.
* https://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-2-dont-put-everything-in-main
*/
void ppg_render_texture(ppg *game, uint32_t cur_tex, int x, int y, SDL_Rect *clip) {

  /* Setup the destination rectangle to be at the position we want */
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;

  if (clip) {
    dst.h = clip->h;
    dst.w = clip->w;
  } else {
    /* Query the texture to get its width and height to use */
    SDL_QueryTexture(game->texture[cur_tex].tex, NULL, NULL, &dst.w, &dst.h);
  }

  /* Copy a portion of the texture to the current rendering target. */
  SDL_RenderCopy(game->ren, game->texture[cur_tex].tex, clip, &dst);
}

/* Render Texture width, height */
void ppg_render_texture_wh(ppg *game, uint32_t cur_tex, int x, int y, int w, int h) {
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = w;
  dst.h = h;

  /* Copy a portion of the texture to the current rendering target. */
  SDL_RenderCopy(game->ren, game->texture[cur_tex].tex, NULL, &dst);
}

void ppg_screen_refresh(ppg *game) {
  SDL_RenderClear(game->ren); /* First clear the renderer */
  ppg_render_texture_wh(game, 1, game->ball.box.x, game->ball.box.y,
                        game->ball.box.w, game->ball.box.h); /* Ball */
  ppg_render_texture_wh(game, 0, game->player.box.x, game->player.box.y,
                        game->player.box.w, game->player.box.h); /* Paddle */
  SDL_RenderPresent(game->ren); /* Update the screen */
}

void regulate_fps() {
  uint8_t start = 0x44;
  uint16_t od_fps = 1000 / FPS;
  uint16_t time = SDL_GetTicks() - start;
  if (od_fps > time) SDL_Delay(od_fps - time);
}
