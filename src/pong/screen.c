#include <stdarg.h>
#include <common.h>
#include <ppg.h>

static bool bmp(ppg *game, uint32_t cur_tex, const char *path) {
  game->texture[cur_tex].surf = SDL_LoadBMP(path);
  if (game->texture[cur_tex].surf) {
    game->texture[cur_tex].tex = SDL_CreateTextureFromSurface(game->ren, game->texture[cur_tex].surf);
    SDL_FreeSurface(game->texture[cur_tex].surf); game->texture[cur_tex].surf = NULL;
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
    ppg_log_me(PPG_DANGER, "[x] IMG_LoadTexture failed, Error: %s", IMG_GetError());
    return false;
  }
  return true;
}

static bool font(ppg *game, uint32_t cur_tex, const char *path, int font_size) {
  /* Open the font (file, font size) */
  game->texture[cur_tex].font = TTF_OpenFont(path, font_size);
  if (!game->texture[cur_tex].font) {
    ppg_log_me(PPG_DANGER, "[x] TTF_OpenFont failed, Error: %s", TTF_GetError());
    return false;
  }

  return true;
}

/* This is very in efficient, but it'll work for what I need to do */
static bool update_text(ppg *game, uint32_t cur_tex, const char *msg) {
  /**
  * We need to first render to a surface as that's what TTF_RenderText
	* returns, then load that surface into a texture
  */
  game->texture[cur_tex].surf = TTF_RenderText_Blended(game->texture[cur_tex].font, msg, game->texture[cur_tex].color);
  if (!game->texture[cur_tex].surf){
    ppg_log_me(PPG_DANGER, "[x] TTF_RenderText_Blended failed, Error: %s", TTF_GetError());
    return false;
  }

  game->texture[cur_tex].tex = SDL_CreateTextureFromSurface(game->ren, game->texture[cur_tex].surf);
  if (!game->texture[cur_tex].tex){
    ppg_log_me(PPG_DANGER, "[x] SDL_CreateTextureFromSurface failed, Error: %s", SDL_GetError());
    return false;
  }

  SDL_FreeSurface(game->texture[cur_tex].surf);
  game->texture[cur_tex].surf = NULL;

  return true;
}

/* Get a variable range of argument to convert and execute in functions bellow */
bool ppg_load_texture(ppg *game, const char *fmt, ...) {
  va_list ap;
  uint32_t cur_tex = 0;
  const char *path = NULL;
  texture_types type;
  int font_size = 0;

  va_start(ap, fmt);
  while (*fmt) {
    /* possible becuase the location that fmt points to is read only */
    switch (*fmt++) {
      case 'i': cur_tex = (int) va_arg(ap, int); break;
      case 't': type = (texture_types) va_arg(ap, texture_types); break;
      case 'p': path = (const char *) va_arg(ap, const char *); break;
      case 'f': font_size = (int) va_arg(ap, int); break;
      default: break;
    }
  }
  va_end(ap);

  switch (type) {
    case PPG_BMP_TEX:
      return !bmp(game, cur_tex, path);
    case PPG_IMG_TEX:
      return !img(game, cur_tex, path);
    case PPG_FONT_TEX:
      return !font(game, cur_tex, path, font_size);
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
void ppg_render_texture_xywh(ppg *game, uint32_t cur_tex, int x, int y, int w, int h) {
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = w;
  dst.h = h;

  /* Copy a portion of the texture to the current rendering target. */
  SDL_RenderCopy(game->ren, game->texture[cur_tex].tex, NULL, &dst);
}

/* Render Game Score */
void ppg_render_texture_text(ppg *game, uint32_t cur_tex) {
  SDL_Rect dst;
  dst.x = 640;
  dst.y = 0;
  dst.w = FONT_SIZE;
  dst.h = 166;

  char msg[30];
  snprintf(msg, 30, "score %d - %d", game->player.points, game->player.points);
  update_text(game, cur_tex, msg);

  /* Copy a portion of the texture to the current rendering target. */
  SDL_RenderCopy(game->ren, game->texture[cur_tex].tex, NULL, &dst);
  SDL_DestroyTexture(game->texture[cur_tex].tex);
  game->texture[cur_tex].tex = NULL;
}

void ppg_reg_fps() {
  time_t t;
  uint16_t start = time(&t);
  uint16_t od_fps = 1000 / FPS;
  uint16_t ticks = SDL_GetTicks() - start;
  if (od_fps > ticks) SDL_Delay(od_fps - ticks);
}

void ppg_screen_refresh(ppg *game) {
  SDL_RenderClear(game->ren); /* First clear the renderer */
  ppg_render_texture_text(game, 2);
  ppg_render_texture_xywh(game, 1, game->ball.box.x, game->ball.box.y,
                        game->ball.box.w, game->ball.box.h); /* Ball */
  ppg_render_texture_xywh(game, 0, game->player.box.x, game->player.box.y,
                        game->player.box.w, game->player.box.h); /* Paddle */
  SDL_RenderPresent(game->ren); /* Update the screen */
}
