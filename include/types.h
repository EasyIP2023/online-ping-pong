#ifndef PPG_TYPES_H
#define PPG_TYPES_H

#include <const.h>

typedef enum _audio_types {
  PPG_MUSIC     = 0x00000000,
  PPG_EFFECT    = 0x00000001,
  PPG_MAX_AUDIO = 0xffffffff
} audio_types;

typedef enum _texture_types {
  PPG_BMP_TEX  = 0x00000000,
  PPG_IMG_TEX  = 0x00000001,
  PPG_FONT_TEX = 0x00000002,
  PPG_MAX_TEX  = 0xffffffff
} texture_types;

typedef enum _otba_types {
  PPG_TEXTURE  = 0x00000000,
  PPG_AUDIO    = 0x00000001,
  PPG_MAX_OTBA = 0xffffffff
} otba_types;

typedef struct _player_t {
  uint32_t y_vel;  /* player only has a y velocity */
  uint32_t points;
  SDL_Rect box;
  bool terminate;
} player_t;

/**
* (x_vel): ball velocity X
* (y_vel): ball velocity Y
* SDL_Rect(box):
*   (x,y): position cordinate data
*   (w,h): Width and height of the image
*/
typedef struct _ball_t {
  uint32_t x_vel, y_vel;   /* x and y velocity */
  SDL_Rect box;
} ball_t;

typedef struct _gdata_t {
  struct {
    int y;
  } send_ppos;
  struct {
    int x, y;
  } send_ball;
  bool terminate;
} gdata_t;

typedef struct _ppg {
  SDL_Window *win;
  SDL_Renderer *ren;

  uint32_t di_size;
  struct _display_item {
    SDL_Surface *surf;
    SDL_Texture *tex;
    SDL_Color color;
    TTF_Font *font;
  } *display_items;

  uint32_t asize;
  struct _audio {
    Mix_Chunk *effect;
    Mix_Music *music;
  } *audio;

  player_t player[2];
  ball_t ball;
} ppg;

#endif
