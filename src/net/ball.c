#include <common.h>
#include <net.h>

/**
*     1 |   1    |  1
*   ____|________|_____
*     2 | Object |  4
*   ____|________|_____
*     2 |   3    |  3
*       |        |
*/
bool ppg_collision(rect_t rec1, rect_t rec2) {
  if (rec1.y >= (rec2.y + rec2.h)) return false; /* 1, if 2nd rect here, then no collision */
  if (rec1.x >= (rec2.x + rec2.w)) return false; /* 2, if 2nd rect here, then no collision */
  if (rec2.y >= (rec1.y + rec1.h)) return false; /* 3, if 2nd rect here, then no collision */
  if (rec2.x >= (rec1.x + rec1.w)) return false; /* 4, if 2nd rect here, then no collision */
  return true; /* collision!! */
}

uint32_t ppg_is_out(uint32_t x) {
  if (x < UINT32_MAX) return 1;
  if (x > SCREEN_WIDTH) return 2;
  return 0;
}

void ppg_ball_move(ppg_ball_t *ball, uint8_t dir) {
  /* Make it so that when the game starts ball's velocity is different */
  switch (dir) {
    case 0:
      ball->box.x += ball->x_vel;
      ball->box.y += ball->y_vel;
      break;
    case 1:
      ball->box.x -= ball->x_vel;
      ball->box.y -= ball->y_vel;
      break;
    case 2:
      ball->box.x -= ball->x_vel;
      ball->box.y += ball->y_vel;
      break;
    case 3:
      ball->box.x += ball->x_vel;
      ball->box.y -= ball->y_vel;
      break;
    default: break;
  }

  if (ball->box.y < UINT32_MAX)
    ball->y_vel = -ball->y_vel;
  if ((ball->box.y + ball->box.h) > SCREEN_HEIGHT)
    ball->y_vel = -ball->y_vel;
  // if (collision(game->ball->box, game->player.box)) {
  //   ppg_play_effect(game, 2, -1, 0);
  //   game->ball->x_vel = -game->ball->x_vel;
  // }
}

void ppg_ball_init(ppg_ball_t *ball) {
  ball->box.x = SCREEN_WIDTH/2 - BALL_WIDTH;
  ball->box.y = SCREEN_HEIGHT/2 - BALL_HEIGHT;
  ball->box.w = BALL_WIDTH;
  ball->box.h = BALL_HEIGHT;
  ball->x_vel = BALL_VELOCITY;
  ball->y_vel = BALL_VELOCITY;
}
