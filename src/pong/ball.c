#include <common.h>
#include <ppg.h>

void ppg_ball_init(ppg *game, int x, int y, int w, int h, int x_vel, int y_vel) {
  game->ball.box.x = x;
  game->ball.box.y = y;
  game->ball.box.w = w;
  game->ball.box.h = h;
  game->ball.x_vel = x_vel;
  game->ball.y_vel = y_vel;
}

void ppg_ball_move(ppg *game) {
  game->ball.box.x += game->ball.x_vel;
  game->ball.box.y += game->ball.y_vel;
}

/**
*     1 |   1    |  1
*   ____|________|_____
*     2 | Object |  4
*   ____|________|_____
*     2 |   3    |  3
*       |        |
*/
bool ppg_ball_collision(SDL_Rect *rec1, SDL_Rect *rec2) {
  if (rec1->y >= (rec2->y + rec2->h)) return false; /* 1, if 2nd rect here, then no collision */
  if (rec1->x >= (rec2->x + rec2->w)) return false; /* 2, if 2nd rect here, then no collision */
  if (rec2->y >= (rec1->y + rec1->h)) return false; /* 3, if 2nd rect here, then no collision */
  if (rec2->x >= (rec1->x + rec1->w)) return false; /* 4, if 2nd rect here, then no collision */
  return true; /* collision!! */
}
