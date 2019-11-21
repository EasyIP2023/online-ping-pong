#ifndef PPG_H
#define PPG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <types.h>

#define FPS 60
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define BALL_WIDTH 50
#define BALL_HEIGHT 50
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 150
#define KEY_RELEASED 900
#define FONT_SIZE 500

/* in src/pong/screen.c */
void ppg_screen_refresh(ppg *game);
void ppg_render_texture(ppg *game, uint32_t cur_tex, int x, int y, SDL_Rect *clip);
void ppg_render_texture_xywh(ppg *game, uint32_t cur_tex, int x, int y, int w, int h);
void ppg_render_texture_text(ppg *game, uint32_t cur_tex);
bool ppg_load_texture(ppg *game, const char *fmt, ...);
bool ppg_poll_ev(SDL_Event *e, int *key_press);
void ppg_reg_fps();

/* in src/pong/ball.c, ball.img is W: 35px H: 35px */
int ppg_is_out(ppg *game);
void ppg_ball_move(ppg *game, uint8_t dir);
void ppg_ball_init(ppg *game, int x, int y, int w, int h, int x_vel, int y_vel);

/* in src/pong/player.c, paddle.img is W: 30px H: 150px */
void ppg_player_init(ppg *game, int x, int y, int w, int h, int y_vel);
void ppg_player_move_up(ppg *game);
void ppg_player_move_down(ppg *game);

/* in src/pong/audio.c */
bool ppg_load_music(ppg *game, const char *music, const char *effect);

/* in src/setup.c */
void ppg_freeup_game(ppg *game);
void ppg_reset_values(ppg *game);
bool ppg_otba(ppg *game, uint32_t size, otba_types type);

#endif
