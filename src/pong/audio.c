#include <common.h>
#include <ppg.h>

bool ppg_load_audio(ppg *game, uint32_t cur_a, const char *audio, audio_types type) {
  /**
  * Initialize the mixer API.
  * This must be called before using other functions in this library.
  */
  if (cur_a == 0) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)) {
      ppg_log_me(PPG_DANGER, "[x] Mix_OpenAudio failed: %s", Mix_GetError());
      return false;
    }
  }

  switch (type) {
    case PPG_MUSIC:
      /* Load music file to use. This can load WAVE, MOD, MIDI, OGG, MP3, FLAC */
      game->audio[cur_a].music = Mix_LoadMUS(audio);
      if (!game->audio[cur_a].music) {
        ppg_log_me(PPG_DANGER, "[x] Mix_LoadMUS failed: %s", Mix_GetError());
        return false;
      }
      break;
    case PPG_EFFECT:
      game->audio[cur_a].effect = Mix_LoadWAV(audio);
      if (!game->audio[cur_a].effect) {
        ppg_log_me(PPG_DANGER, "[x] Mix_LoadWAV failed: %s", Mix_GetError());
        return false;
      }
      break;
    default: break;
  }

  return true;
}

/**
* Play the loaded music looptimes through from start to finish.
* he previous music will behalted, or if fading out it waits
* (blocking) for that to finish
*/
bool ppg_play_music(ppg *game, uint32_t cur_a, int loop) {
  if (Mix_PlayMusic(game->audio[cur_a].music, loop)) {
    ppg_log_me(PPG_DANGER, "[x] Mix_PlayMusic failed: %s", Mix_GetError());
    return false;
  }
  return true;
}

/* Play chunk/wav/music/SE on channel, or if channel is -1, pick the first free unreserved channel. */
bool ppg_play_effect(ppg *game, uint32_t cur_a, int channel, int loop) {
  if (Mix_PlayChannel(channel, game->audio[cur_a].effect, loop)) {
    ppg_log_me(PPG_DANGER, "[x] Mix_PlayMusic failed: %s", Mix_GetError());
    return false;
  }
  return true;
}
