#ifndef PPG_TYPES_H
#define PPG_TYPES_H

#define FPS 60
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define BALL_WIDTH 50
#define BALL_HEIGHT 50
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 150
#define KEY_RELEASED 900
#define FONT_SIZE 500
#define MOUSE_BUTTON_PRESSED 645
#define EXIT_GAME 123
#define PLAY_GAME 345
#define RET_TO_MENU 657

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

typedef struct _ppg {
  SDL_Window *win;
  SDL_Renderer *ren;

  uint32_t tsize;
  struct _display_item {
    SDL_Surface *surf;
    SDL_Texture *tex;
    SDL_Color color;
    TTF_Font *font;
  } *display_items;

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
