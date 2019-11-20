#ifndef PPG_H
#define PPG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <types.h>

#define FPS 30
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define BALL_WIDTH 50
#define BALL_HEIGHT 50
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 150
#define KEY_RELEASED 900

void ppg_screen_refresh(ppg *game);
void ppg_render_texture(ppg *game, uint32_t cur_tex, int x, int y, SDL_Rect *clip);
void ppg_render_texture_wh(ppg *game, uint32_t cur_tex, int x, int y, int w, int h);
bool ppg_load_texture(ppg *game, uint32_t cur_tex, const char *file, texture_types type);
bool ppg_poll_ev(SDL_Event *e, int *key_press);

/* in src/pong/ball.c, ball.img is W: 35px H: 35px */
void ppg_ball_move(ppg *game, uint8_t dir);
void ppg_ball_init(ppg *game, int x, int y, int w, int h, int x_vel, int y_vel);

/* in src/pong/player.c, paddle.img is W: 30px H: 150px */
void ppg_player_init(ppg *game, int x, int y, int w, int h, int y_vel);
void ppg_player_move_up(ppg *game);
void ppg_player_move_down(ppg *game);

void ppg_freeup_game(ppg *game);
void ppg_reset_values(ppg *game);
bool ppg_otba(ppg *game, uint32_t size, otba_types type);

#endif
