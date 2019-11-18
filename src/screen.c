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
* Whilst preserving the texture's width and height
* https://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-2-dont-put-everything-in-main
*/
void render_texture(ppg *game, uint32_t cur_tex, int x, int y) {

  /* Setup the destination rectangle to be at the position we want */
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;

  /* Query the texture to get its width and height to use */
  SDL_QueryTexture(game->texture[cur_tex].tex, NULL, NULL, &dst.w, &dst.h);
  SDL_RenderCopy(game->ren, game->texture[cur_tex].tex, NULL, &dst);
}

/* Render Texture width, height */
void render_texture_wh(ppg *game, uint32_t cur_tex, int x, int y, int w, int h) {
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = w;
  dst.h = h;

  /* Query the texture to get its width and height to use */
  SDL_RenderCopy(game->ren, game->texture[cur_tex].tex, NULL, &dst);
}
