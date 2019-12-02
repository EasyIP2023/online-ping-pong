#include <common.h>
#include <ppg.h>

/**
*     1 |   1    |  1
*   ____|________|_____
*     2 | Object |  4
*   ____|________|_____
*     2 |   3    |  3
*       |        |
*/
static bool collision(SDL_Rect rec1, SDL_Rect rec2) {
  if (rec1.y >= (rec2.y + rec2.h)) return false; /* 1, if 2nd rect here, then no collision */
  if (rec1.x >= (rec2.x + rec2.w)) return false; /* 2, if 2nd rect here, then no collision */
  if (rec2.y >= (rec1.y + rec1.h)) return false; /* 3, if 2nd rect here, then no collision */
  if (rec2.x >= (rec1.x + rec1.w)) return false; /* 4, if 2nd rect here, then no collision */
  return true; /* collision!! */
}

uint32_t ppg_is_out(ppg *game) {
  if (game->g_data.ball.box.x < 0) return 1;
  if (game->g_data.ball.box.x > SCREEN_WIDTH) return 2;
  return 0;
}

void ppg_ball_move(ppg *game, uint8_t dir) {
  /* Make it so that when the game starts ball's velocity is different */
  switch (dir) {
    case 0:
      game->g_data.ball.box.x += game->g_data.ball.x_vel;
      game->g_data.ball.box.y += game->g_data.ball.y_vel;
      break;
    case 1:
      game->g_data.ball.box.x -= game->g_data.ball.x_vel;
      game->g_data.ball.box.y -= game->g_data.ball.y_vel;
      break;
    case 2:
      game->g_data.ball.box.x -= game->g_data.ball.x_vel;
      game->g_data.ball.box.y += game->g_data.ball.y_vel;
      break;
    case 3:
      game->g_data.ball.box.x += game->g_data.ball.x_vel;
      game->g_data.ball.box.y -= game->g_data.ball.y_vel;
      break;
    default: break;
  }
  if (game->g_data.ball.box.y < 0)
    game->g_data.ball.y_vel = -game->g_data.ball.y_vel;
  if ((game->g_data.ball.box.y + game->g_data.ball.box.h) > SCREEN_HEIGHT)
    game->g_data.ball.y_vel = -game->g_data.ball.y_vel;
  if (collision(game->g_data.ball.box, game->g_data.player[0].box)) {
    ppg_play_effect(game, 2, -1, 0);
    game->g_data.ball.x_vel = -game->g_data.ball.x_vel;
  }
  if (collision(game->g_data.ball.box, game->g_data.player[1].box)) {
    ppg_play_effect(game, 2, -1, 0);
    game->g_data.ball.x_vel = -game->g_data.ball.x_vel;
  }
}

void ppg_ball_init(ppg *game) {
  game->g_data.ball.box.x = SCREEN_WIDTH/2 - BALL_WIDTH;
  game->g_data.ball.box.y = SCREEN_HEIGHT/2 - BALL_HEIGHT;
  game->g_data.ball.box.w = BALL_WIDTH;
  game->g_data.ball.box.h = BALL_HEIGHT;
  game->g_data.ball.x_vel = BALL_VELOCITY;
  game->g_data.ball.y_vel = BALL_VELOCITY;
}
