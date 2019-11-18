
#ifndef PPG_H
#define PPG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <types.h>

void ppg_render_texture(ppg *game, uint32_t cur_tex, int x, int y, SDL_Rect *clip);
void ppg_render_texture_wh(ppg *game, uint32_t cur_tex, int x, int y, int w, int h, SDL_Rect *clip);
bool ppg_load_texture(ppg *game, uint32_t cur_tex, const char *file, texture_types type);
bool ppg_otba(ppg *game, uint32_t size, otba_types type);
bool ppg_poll_ev(int *clip);
void freeup_ppg(ppg *game);

#endif
