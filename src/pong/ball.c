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
  if (game->ball.box.x < 0) return 1;
  if (game->ball.box.x > SCREEN_WIDTH) return 2;
  return 0;
}

void ppg_ball_move(ppg *game, uint8_t dir) {
  /* Make it so that when the game starts ball's velocity is different */
  switch (dir) {
    case 0:
      game->ball.box.x += game->ball.x_vel;
      game->ball.box.y += game->ball.y_vel;
      break;
    case 1:
      game->ball.box.x -= game->ball.x_vel;
      game->ball.box.y -= game->ball.y_vel;
      break;
    case 2:
      game->ball.box.x -= game->ball.x_vel;
      game->ball.box.y += game->ball.y_vel;
      break;
    case 3:
      game->ball.box.x += game->ball.x_vel;
      game->ball.box.y -= game->ball.y_vel;
      break;
    default: break;
  }

  if (game->ball.box.y < 0)
    game->ball.y_vel = -game->ball.y_vel;
  if ((game->ball.box.y + game->ball.box.h) > SCREEN_HEIGHT)
    game->ball.y_vel = -game->ball.y_vel;
  if (collision(game->ball.box, game->player[0].box)) {
    ppg_play_effect(game, 2, -1, 0);
    game->ball.x_vel = -game->ball.x_vel;
  }
  if (collision(game->ball.box, game->player[1].box)) {
    ppg_play_effect(game, 2, -1, 0);
    game->ball.x_vel = -game->ball.x_vel;
  }
}

void ppg_ball_init(ppg *game) {
  game->ball.box.x = SCREEN_WIDTH/2 - BALL_WIDTH;
  game->ball.box.y = SCREEN_HEIGHT/2 - BALL_HEIGHT;
  game->ball.box.w = BALL_WIDTH;
  game->ball.box.h = BALL_HEIGHT;
  game->ball.x_vel = BALL_VELOCITY;
  game->ball.y_vel = BALL_VELOCITY;
}
