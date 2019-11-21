
#ifndef PPG_TYPES_H
#define PPG_TYPES_H

typedef enum _texture_types {
  PPG_BMP_TEX = 0,
  PPG_IMG_TEX = 1,
  PPG_FONT_TEX = 2
} texture_types;

typedef enum _otba_types {
  PPG_TEXTURE = 0
} otba_types;

typedef struct _ppg {
  SDL_Window *win;
  SDL_Renderer *ren;

  uint32_t tsize;
  struct _texture {
    SDL_Surface *surf;
    SDL_Texture *tex;
    SDL_Color color;
    TTF_Font *font;
  } *texture;

  struct _ball {
    int x_vel, y_vel;   /* x and y velocity */
    SDL_Rect box;
  } ball;

  struct _paddle {
    int y_vel;  /* paddle only has a y velocity */
    int points;
    SDL_Rect box;
  } player;

  Mix_Music *music;
  Mix_Chunk *chunk;
} ppg;

#endif
