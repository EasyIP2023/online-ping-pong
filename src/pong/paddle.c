#include <common.h>
#include <ppg.h>

void ppg_player_init(ppg *game, int x, int y, int w, int h, int y_vel) {
  game->player.box.x = x;
  game->player.box.y = y;
  game->player.box.w = w;
  game->player.box.h = h;
  game->player.y_vel = y_vel;
}

void ppg_player_move_up(ppg *game) {
  if (game->player.box.y > 0)
    game->player.box.y -= game->player.y_vel;
}

void ppg_player_move_down(ppg *game) {
  if ((game->player.box.y + game->player.box.h) < SCREEN_HEIGHT)
    game->player.box.y += game->player.y_vel;
}
