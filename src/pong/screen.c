#include <common.h>
#include <ppg.h>

static bool bmp(ppg *game, uint32_t cur_di, const char *path) {
  game->display_items[cur_di].surf = SDL_LoadBMP(path);
  if (game->display_items[cur_di].surf) {
    game->display_items[cur_di].tex = SDL_CreateTextureFromSurface(game->ren, game->display_items[cur_di].surf);
    SDL_FreeSurface(game->display_items[cur_di].surf); game->display_items[cur_di].surf = NULL;
    if (!game->display_items[cur_di].tex) {
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

static bool img(ppg *game, uint32_t cur_di, const char *path) {
  if (!game->ren) {
    ppg_log_me(PPG_DANGER, "[x] must make a call to SDL_CreateRenderer()");
    return false;
  }
  game->display_items[cur_di].tex = IMG_LoadTexture(game->ren, path);
  if (!game->display_items[cur_di].tex) {
    ppg_log_me(PPG_DANGER, "[x] IMG_LoadTexture failed, Error: %s", IMG_GetError());
    return false;
  }
  return true;
}

static bool font(ppg *game, uint32_t cur_di, const char *path, int font_size) {
  /* Open the font (file, font size) */
  game->display_items[cur_di].font = TTF_OpenFont(path, font_size);
  if (!game->display_items[cur_di].font) {
    ppg_log_me(PPG_DANGER, "[x] TTF_OpenFont failed, Error: %s", TTF_GetError());
    return false;
  }

  return true;
}

/**
* Wanted font updating to work with my rendering target
* So I decided to create a surface from the font and message
* Create a texture from that surface
* Copy texture to the current rendering target
*/
static bool ppg_render_texture_text(ppg *game, uint32_t cur_di, SDL_Rect *dst, const char *msg) {
  bool ret = false;

  game->display_items[cur_di].surf = TTF_RenderText_Blended(game->display_items[cur_di].font, msg, game->display_items[cur_di].color);
  if (!game->display_items[cur_di].surf){
    ppg_log_me(PPG_DANGER, "[x] TTF_RenderText_Blended failed, Error: %s", TTF_GetError());
    return ret;
  }

  game->display_items[cur_di].tex = SDL_CreateTextureFromSurface(game->ren, game->display_items[cur_di].surf);
  if (!game->display_items[cur_di].tex){
    ppg_log_me(PPG_DANGER, "[x] SDL_CreateTextureFromSurface failed, Error: %s", SDL_GetError());
    return ret;
  }

  SDL_FreeSurface(game->display_items[cur_di].surf);
  game->display_items[cur_di].surf = NULL;

  ret = SDL_RenderCopy(game->ren, game->display_items[cur_di].tex, NULL, dst);
  if (ret) {
    ppg_log_me(PPG_DANGER, "[x] SDL_CreateTextureFromSurface failed, Error: %s", SDL_GetError());
    return ret;
  }

  SDL_DestroyTexture(game->display_items[cur_di].tex);
  game->display_items[cur_di].tex = NULL;

  return ret;
}

void ppg_copy_sdl_color(ppg *game, uint32_t cur_di, SDL_Color color) {
  /* Now in font display item */
  game->display_items[cur_di].color.r = color.r;
  game->display_items[cur_di].color.b = color.b;
  game->display_items[cur_di].color.g = color.g;
  game->display_items[cur_di].color.a = color.a;
}

/* Get a variable range of argument to convert and execute in functions bellow */
bool ppg_load_display_item(ppg *game, const char *fmt, ...) {
  va_list ap;
  uint32_t cur_di = 0;
  const char *path = NULL;
  texture_types type;
  int font_size = 0;

  va_start(ap, fmt);
  while (*fmt) { /* Leave this way no need to add visibility brackets {} */
    /* possible becuase the location that fmt points to is read only */
    switch (*fmt++) {
      case 'i': cur_di = (int) va_arg(ap, int); break;
      case 't': type = (texture_types) va_arg(ap, texture_types); break;
      case 'p': path = (const char *) va_arg(ap, const char *); break;
      case 'f': font_size = (int) va_arg(ap, int); break;
      default: break;
    }
  }
  va_end(ap);

  switch (type) {
    case PPG_BMP_TEX:
      return !bmp(game, cur_di, path);
    case PPG_IMG_TEX:
      return !img(game, cur_di, path);
    case PPG_FONT_TEX:
      return !font(game, cur_di, path, font_size);
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
void ppg_render_texture(ppg *game, uint32_t cur_di, int x, int y, SDL_Rect *clip) {

  /* Setup the destination rectangle to be at the position we want */
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;

  if (clip) {
    dst.h = clip->h;
    dst.w = clip->w;
  } else {
    /* Query the texture to get its width and height to use */
    SDL_QueryTexture(game->display_items[cur_di].tex, NULL, NULL, &dst.w, &dst.h);
  }

  /* Copy a portion of the texture to the current rendering target. */
  SDL_RenderCopy(game->ren, game->display_items[cur_di].tex, clip, &dst);
}

/* Render Texture width, height */
bool ppg_render_texture_xywh(ppg *game, uint32_t cur_di, int x, int y, int w, int h) {
  bool ret = false;
  SDL_Rect dst = {x, y, w, h};

  /* Copy a portion of the texture to the current rendering target. */
  ret = SDL_RenderCopy(game->ren, game->display_items[cur_di].tex, NULL, &dst);
  if (ret) {
    ppg_log_me(PPG_DANGER, "[x] SDL_CreateTextureFromSurface failed, Error: %s", SDL_GetError());
    return ret;
  }

  return ret;
}

void ppg_reg_fps() {
  time_t t;
  uint16_t start = time(&t);
  uint16_t od_fps = 1000 / FPS;
  uint16_t ticks = SDL_GetTicks() - start;
  if (od_fps > ticks) SDL_Delay(od_fps - ticks);
}

bool ppg_screen_refresh(ppg *game, bool player_found) {
  bool ret = false;
  SDL_Color colors[2] = {{26, 255, 26, 0}, {255, 255, 0, 0}};
  SDL_Rect dst = {640, 0, FONT_SIZE, 166};

  SDL_RenderClear(game->ren); /* First clear the renderer */

  /* Render Game Score */
  char msg[30];
  if (player_found) {
    ppg_copy_sdl_color(game, 2, colors[0]);
    snprintf(msg, 30, "score %d - %d", game->player[0].points, game->player[1].points);
  } else {
    ppg_copy_sdl_color(game, 2, colors[1]);
    snprintf(msg, 30, "Waiting for player 2");
  }

  ret = ppg_render_texture_text(game, 2, &dst, msg);
  if (ret) return ret;

  ret = ppg_render_texture_xywh(game, 1, game->ball.box.x, game->ball.box.y, game->ball.box.w, game->ball.box.h);
  if (ret) return ret;

  ret = ppg_render_texture_xywh(game, 0, game->player[0].box.x, game->player[0].box.y, game->player[0].box.w, game->player[0].box.h);
  if (ret) return ret;

  ret = ppg_render_texture_xywh(game, 0, game->player[1].box.x, game->player[1].box.y, game->player[1].box.w, game->player[1].box.h);
  if (ret) return ret;

  SDL_RenderPresent(game->ren); /* Update the screen */

  return ret;
}

static void update_position(SDL_Rect *pos, int x, int y, int w, int h) {
  pos->x = x;
  pos->y = y;
  pos->w = w;
  pos->h = h;
}

static int entered_play_label(SDL_Event *e) {
  return (e->motion.x <= (SCREEN_WIDTH/2 + 200) && e->motion.x >= (SCREEN_WIDTH/2 - 200)) &&
         (e->motion.y <= (SCREEN_HEIGHT/2 + 10) && e->motion.y >= (SCREEN_HEIGHT/2 - 120));
}

static bool entered_exit_label(SDL_Event *e) {
  return (e->motion.x <= (SCREEN_WIDTH/2 + 200) && e->motion.x >= (SCREEN_WIDTH/2 - 200)) &&
         (e->motion.y <= (SCREEN_HEIGHT/2 + 150) && e->motion.y >= (SCREEN_HEIGHT/2));
}

/* Hacked up but she works */
bool ppg_show_menu(ppg *game, SDL_Event *e, uint32_t *key) {
  bool ret = false;
  const char *labels[3] = {"Online Ping Pong", "Play Game", "Exit"};
  SDL_Color colors[2] = {{255,255,255,0}, {255,0,0,0}};

  /* Position Menu Items */
  SDL_Rect pos[3];
  ppg_copy_sdl_color(game, 3, colors[0]);
  update_position(&pos[0], 640, 0, FONT_SIZE+40, 166);
  update_position(&pos[1], SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 150, 300, 150);
  update_position(&pos[2], SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2, 300, 150);

  SDL_RenderClear(game->ren); /* First clear the renderer */

  /* In the font game menu display item */
  ret = ppg_render_texture_text(game, 3, &pos[0], labels[0]);
  if (ret) return ret;

  ret = ppg_render_texture_text(game, 3, &pos[1], labels[1]);
  if (ret) return ret;

  ret = ppg_render_texture_text(game, 3, &pos[2], labels[2]);
  if (ret) return ret;

  if (entered_exit_label(e)) {
    ppg_copy_sdl_color(game, 3, colors[1]);
    ret = ppg_render_texture_text(game, 3, &pos[2], labels[2]);
    if (ret) return ret;
    if (*key == MOUSE_BUTTON_PRESSED) *key = EXIT_GAME;
  } else if (entered_play_label(e)) {
    ppg_copy_sdl_color(game, 3, colors[1]);
    ret = ppg_render_texture_text(game, 3, &pos[1], labels[1]);
    if (ret) return ret;
    if (*key == MOUSE_BUTTON_PRESSED) *key = PLAY_GAME;
  }

  SDL_RenderPresent(game->ren); /* Update the screen */

  return ret;
}
