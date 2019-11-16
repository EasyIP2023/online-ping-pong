#include <common.h>
#include <ppg.h>

int pp_load_texture(ppg *game, char *file) {
	game->tex = NULL;

  char *path = pp_file_path(file);
	game->surf = SDL_LoadBMP(path);
	if (game->surf){
    game->tex = SDL_CreateTextureFromSurface(game->ren, game->surf);
    SDL_FreeSurface(game->surf); game->surf = NULL;
    if (!game->tex){
      pp_log_me(PP_SUCCESS, "SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
      return EXIT_SUCCESS;
    }
    pp_log_me(PP_SUCCESS, "SDL Created Texture From Surface");
	}
	else {
		pp_log_me(PP_DANGER, "Need to load BMP");
    return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
