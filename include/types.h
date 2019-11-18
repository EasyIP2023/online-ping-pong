
#ifndef PPG_TYPES_H
#define PPG_TYPES_H

typedef enum _texture_types {
  PPG_BMP_TEX = 0,
  PPG_IMG_TEX = 1
} texture_types;

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

#endif
