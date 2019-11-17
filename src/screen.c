#include <common.h>
#include <ppg.h>

int ppg_load_texture(ppg *game, uint32_t cur_tex, char *name, char *file) {
  char *path = ppg_file_path(file);
  game->surf = SDL_LoadBMP(path);
  if (game->surf){
    game->texture[cur_tex].name = name;
    game->texture[cur_tex].tex = SDL_CreateTextureFromSurface(game->ren, game->surf);
    SDL_FreeSurface(game->surf); game->surf = NULL;
    if (!game->texture[cur_tex].tex) {
      ppg_log_me(PPG_DANGER, "SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
      return EXIT_SUCCESS;
    }
    ppg_log_me(PPG_SUCCESS, "SDL Created Texture From Surface");
	}
  else {
    ppg_log_me(PPG_DANGER, "Failed to load a BMP");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/*
 * Draw an SDL_Texture to an SDL_Renderer at position x, y.
 * Whilst preserving the texture's width and height
 * (https://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-2-dont-put-everything-in-main)
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
