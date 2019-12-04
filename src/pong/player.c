#include <common.h>
#include <ppg.h>

void ppg_player_init(ppg *game, uint32_t cur_p, uint32_t x, uint32_t y) {
	game->player[cur_p].box.x = x;
	game->player[cur_p].box.y = y;
	game->player[cur_p].box.w = PLAYER_WIDTH;
	game->player[cur_p].box.h = PLAYER_HEIGHT;
	game->player[cur_p].y_vel = PLAYER_VELOCITY;
  game->player[cur_p].terminate = false;
}

void ppg_player_move_up(ppg *game, uint32_t cur_p) {
	if (game->player[cur_p].box.y > 0)
		game->player[cur_p].box.y -= game->player[cur_p].y_vel;
}

void ppg_player_move_down(ppg *game, uint32_t cur_p) {
	if ((game->player[cur_p].box.y + game->player[cur_p].box.h) < SCREEN_HEIGHT)
		game->player[cur_p].box.y += game->player[cur_p].y_vel;
}
