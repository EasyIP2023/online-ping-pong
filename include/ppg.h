
#ifndef PPG_H
#define PPG_H

#include <SDL2/SDL.h>

typedef struct _ppg {
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *tex;
  SDL_Surface *surf;
} ppg;

int pp_load_texture(ppg *game, char *file);
void freeup_ppg(ppg *game);

#endif
