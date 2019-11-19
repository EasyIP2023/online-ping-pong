#ifndef PPG_H
#define PPG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <types.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 935
#define BALL_WIDTH 50
#define BALL_HEIGHT 50
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 150
#define KEY_RELEASED 900

void ppg_render_texture(ppg *game, uint32_t cur_tex, int x, int y, SDL_Rect *clip);
void ppg_render_texture_wh(ppg *game, uint32_t cur_tex, int x, int y, int w, int h, SDL_Rect *clip);
bool ppg_load_texture(ppg *game, uint32_t cur_tex, const char *file, texture_types type);
void ppg_screen_refresh(ppg *game, uint32_t ball, uint32_t paddle);
bool ppg_otba(ppg *game, uint32_t size, otba_types type);
bool ppg_poll_ev(SDL_Event *e, int *key_press);

/* in src/pong/ball.c */ // ball.img is W: 35px H: 35px
void ppg_ball_init(ppg *game, int x, int y, int w, int h, int x_vel, int y_vel);
void ppg_ball_move(ppg *game);
bool ppg_ball_collision(SDL_Rect *rec1, SDL_Rect *rec2);

/* in src/pong/player.c */ // paddle.img is W: 30px H: 150px
void ppg_player_init(ppg *game, int x, int y, int w, int h, int y_vel);
bool ppg_player_move_up(ppg *game);
bool ppg_player_move_down(ppg *game);

void freeup_ppg(ppg *game);

#endif
