
#ifndef PPG_H
#define PPG_H

#include <SDL2/SDL.h>

typedef enum _otba_types {
  PPG_TEXTURE = 0
} otba_types;

typedef struct _ppg {
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Surface *surf;

  uint32_t tsize;
  struct texture {
    char *name;
    SDL_Texture *tex;
  } *texture;
} ppg;

int ppg_load_texture(ppg *game, uint32_t cur_tex, char *name, char *file);
void render_texture(ppg *game, uint32_t cur_tex, int x, int y);
int ppg_otba(ppg *game, uint32_t size, otba_types type);
void freeup_ppg(ppg *game);

#endif
