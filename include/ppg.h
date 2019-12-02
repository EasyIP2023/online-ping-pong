#ifndef PPG_H
#define PPG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <types.h>

/* in src/pong/screen.c */
bool ppg_screen_refresh(ppg *game, bool player_found);
void ppg_render_texture(ppg *game, uint32_t cur_di, int x, int y, SDL_Rect *clip);
bool ppg_render_texture_xywh(ppg *game, uint32_t cur_di, int x, int y, int w, int h);
void ppg_copy_sdl_color(ppg *game, uint32_t cur_di, SDL_Color color);
bool ppg_load_display_item(ppg *game, const char *fmt, ...);
bool ppg_poll_ev(SDL_Event *e, uint32_t *key);
bool ppg_show_menu(ppg *game, SDL_Event *e, uint32_t *key);
void ppg_reg_fps();

/* in src/pong/player.c, paddle.img is W: 30px H: 150px */
void ppg_player_init(ppg *game, uint32_t cur_p, uint32_t x, uint32_t y);
void ppg_player_move_up(ppg *game, uint32_t cur_p);
void ppg_player_move_down(ppg *game, uint32_t cur_p);

/* in src/pong/audio.c */
bool ppg_load_audio(ppg *game, uint32_t cur_a, const char *audio, audio_types type);
bool ppg_play_music(ppg *game, uint32_t cur_a, int loop);
bool ppg_play_effect(ppg *game, uint32_t cur_a, int channel, int loop);

/* in src/pong/ball.c, ball.img is W: 35px H: 35px */
uint32_t ppg_is_out(ppg *game);
void ppg_ball_move(ppg *game, uint8_t dir);
void ppg_ball_init(ppg *game);
bool ppg_collision(SDL_Rect rec1, SDL_Rect rec2);

/* in src/setup.c */
void ppg_freeup_game(ppg *game);
void ppg_reset_values(ppg *game);
bool ppg_otba(ppg *game, uint32_t size, otba_types type);

#endif
