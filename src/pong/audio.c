#include <common.h>
#include <ppg.h>

bool ppg_load_music(ppg *game, const char *music, const char *effect) {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)) {
    ppg_log_me(PPG_DANGER, "[x] Mix_OpenAudio failed: %s", Mix_GetError());
    return false;
  }

  game->music = Mix_LoadMUS(music);
  if (!game->music) {
    ppg_log_me(PPG_DANGER, "[x] Mix_LoadMUS failed: %s", Mix_GetError());
    return false;
  }

  ppg_log_me(PPG_WARNING, "%s", effect);

  // game->chunk = Mix_LoadWAV(effect);
  // if (!game->chunk) {
  //   ppg_log_me(PPG_DANGER, "[x] Mix_LoadWAV failed: %s", Mix_GetError());
  //   return false;
  // }

  Mix_PlayMusic(game->music, -1);

  return true;
}
