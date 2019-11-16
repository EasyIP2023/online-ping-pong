#include <common.h>
#include <ppg.h>

void freeup_ppg(ppg *game) {
  if (game->win)
    SDL_DestroyWindow(game->win);
  if (game->surf)
    SDL_FreeSurface(game->surf);
  if (game->ren)
    SDL_DestroyRenderer(game->ren);
  if (game->surf)
    SDL_FreeSurface(game->surf);

  SDL_Quit();
}
