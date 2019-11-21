
#ifndef PPG_TYPES_H
#define PPG_TYPES_H

typedef enum _audio_types {
  PPG_MUSIC = 0,
  PPG_EFFECT = 1
} audio_types;

typedef enum _texture_types {
  PPG_BMP_TEX = 0,
  PPG_IMG_TEX = 1,
  PPG_FONT_TEX = 2
} texture_types;

typedef enum _otba_types {
  PPG_TEXTURE = 0,
  PPG_AUDIO = 1
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

  uint32_t asize;
  struct _audio {
    Mix_Chunk *effect;
    Mix_Music *music;
  } *audio;
} ppg;

#endif
