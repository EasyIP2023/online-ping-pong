#include <common.h>
#include <ppg.h>
#include <net.h>

#include <netpong.h>

static bool load_display_items(ppg *game) {
  bool err = false;
  uint32_t cur_di = 0;
  err = ppg_load_display_item(game, "i:p:t", cur_di, "imgs/paddle.png", PPG_IMG_TEX);
  if (err) return err;
  cur_di++;

  err = ppg_load_display_item(game,"i:p:t", cur_di, "imgs/ball.png", PPG_IMG_TEX);
  if (err) return err;
  cur_di++;

  /* Load score font */
  err = ppg_load_display_item(game, "i:p:t:f", cur_di, "fonts/bankruptcy/Bankruptcy.otf", PPG_FONT_TEX, FONT_SIZE);
  if (err) return err;
  cur_di++;

  /* Load menu font */
  err = ppg_load_display_item(game, "i:p:t:f", cur_di, "fonts/bankruptcy/Bankruptcy.otf", PPG_FONT_TEX, FONT_SIZE);
  if (err) return err;

  return err;
}

static void player_two_reset(ppg *game) {
  ppg_player_init(game, 1, SCREEN_WIDTH - PLAYER_WIDTH, SCREEN_HEIGHT/2);
  ppg_ball_init(game);
}

static void player_one_reset(ppg *game) {
  ppg_player_init(game, 0, 0, SCREEN_HEIGHT/2 - PLAYER_HEIGHT);
  ppg_ball_init(game);
}

/**
* read data from server and update
* player struct members to present on screen
* Ball position is updated and sent to player 2
* player 2 then updates it on their screen
*/
static void recv_data(uint32_t sock, ppg *game, uint8_t player) {
  gdata_t data;

  if (read(sock, &data, sizeof(data)) == ERR64) return;
  switch (player) {
    case 0:
      game->player[1].box.y = data.send_ppos.y;
      game->player[1].terminate = data.terminate;
      break;
    case 1:
      game->ball.box.x = data.send_ball.x;
      game->ball.box.y = data.send_ball.y;
      game->player[0].box.y = data.send_ppos.y;
      game->player[0].terminate = data.terminate;
      break;
    default: return;
  }
}

/* send struct member data to server */
static void send_data(uint32_t sock, ppg *game, uint8_t p) {
  gdata_t data;

  data.send_ppos.y = game->player[p].box.y;
  data.terminate =  game->player[p].terminate;
  if (p == 0) {
    data.send_ball.x = game->ball.box.x;
    data.send_ball.y = game->ball.box.y;
  }

  if (write(sock, &data, sizeof(gdata_t)) == ERR64) return;
}

bool start_client(const char *ip_addr, uint16_t port) {
  bool err = false;
  ppg game;
  ppg_reset_values(&game);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ppg_log_me(PPG_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    return false;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    ppg_log_me(PPG_DANGER, "IMG_Init failed: %s", IMG_GetError());
    ppg_freeup_game(&game);
    return false;
  }

  if (TTF_Init() != 0) {
    ppg_log_me(PPG_DANGER, "TTF_Init failed: %s", TTF_GetError());
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_otba(&game, 4, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_otba(&game, 3, PPG_AUDIO);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ppg_freeup_game(&game);
    return false;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
  if (!game.win) {
    ppg_log_me(PPG_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
    ppg_freeup_game(&game);
    return false;
  }
  ppg_log_me(PPG_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!game.ren){
    ppg_log_me(PPG_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    ppg_freeup_game(&game);
    return false;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Created Renderer");

  if (load_display_items(&game)) {
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_load_audio(&game, 0, "music/evolution.mp3", PPG_MUSIC);
  if (!err) {
    ppg_log_me(PPG_DANGER, "Failed!!");
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_load_audio(&game, 1, "music/dreams.mp3", PPG_MUSIC);
  if (!err) {
    ppg_log_me(PPG_DANGER, "Failed!!");
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_load_audio(&game, 2, "music/mario_jump.wav", PPG_EFFECT);
  if (!err) {
    ppg_log_me(PPG_DANGER, "Failed!!");
    ppg_freeup_game(&game);
    return false;
  }

  /* read user input and handle it */
  uint32_t key = 0, sock_fd = 0;
  SDL_Event e;
  bool game_menu = false, game_over = true, music_playing = false;
  bool run_once = false, found_player = false;
  uint8_t player = 0xff, player_two = 0xff;

  /* Game!! Welcome to crazy If Statement Checks */
  while (!ppg_poll_ev(&e, &key)) {
    if (key == EXIT_GAME) goto exit_game;
    if (key == RET_TO_MENU && player != 0xff) { /* reset values */
      game_over = true;
      music_playing = false;
      game.player[player].terminate = true;
      send_data(sock_fd, &game, player);
      player = player_two = 0xff;
      close(sock_fd);
    }

    if (!game_menu && game_over) {
      if (!music_playing) {
        if (!ppg_play_music(&game, 1, -1)) {
          ppg_freeup_game(&game);
          return false;
        }
        music_playing = true;
      }
      game_menu = ppg_show_menu(&game, &e, &key);
      if (key == PLAY_GAME && player == 0xff) {
        game_over = false;
        music_playing = false;
        sock_fd = ppg_connect_client(ip_addr, port);
        if (sock_fd == UINT32_MAX) { continue; } /* Force the next iteration of the loop */
        else {
          /* Find out what player you are from the server */
          if (read(sock_fd, &player, sizeof(player)) == ERR64) {
            ppg_log_me(PPG_DANGER, "[x] read: %s", strerror(errno));
            game_over = true;
            music_playing = false;
          }

          /* Doing so allows me not to have to create two seperate processes, input_handler & output_handler */
          if (!make_fd_non_blocking(sock_fd)) return false;
          ppg_log_me(PPG_SUCCESS, "connection established, player %u", player);
        }
      }
    }

    /* Actual game */
    if (!game_over && player != 0xff) {
      if (found_player && !run_once) {
        player_two_reset(&game);
        player_one_reset(&game);
        run_once = true;
        ppg_log_me(PPG_WARNING, "Start Game");
      }

      /* Continue to read until player two connected */
      if (!found_player) {
        read(sock_fd, &player_two, sizeof(player_two)); /* Don't check if read call failed */
        found_player = (player != 0xff && player_two != 0xff) ? true : false;
      } else {
        recv_data(sock_fd, &game, player);
        if (game.player[player_two].terminate) key = RET_TO_MENU;
        send_data(sock_fd, &game, player);
      }

      if (!music_playing) {
        music_playing = true;
        if (!ppg_play_music(&game, 0, -1)) {
          ppg_freeup_game(&game);
          return false;
        }
      }

      if (ppg_screen_refresh(&game, found_player)) goto exit_game;
      switch (key) {
        case 4:
          if (key != KEY_RELEASED) ppg_player_move_down(&game, player);
          break;
        case 5:
          if (key != KEY_RELEASED) ppg_player_move_up(&game, player);
          break;
        default: break;
      }

      /* constantly send balls new position */
      if (found_player && player == 0) {
        ppg_ball_move(&game, 2);
        send_data(sock_fd, &game, player);
      }

      /* constantly send/recv balls new position */
      if (found_player) {
        switch (ppg_is_out(&game)) {
          case 1:
            game.player[1].points++;
            player_two_reset(&game);
            player_one_reset(&game);
            break;
          case 2:
            game.player[0].points++;
            player_two_reset(&game);
            player_one_reset(&game);
            break;
          default: break;
        }
      }

      if (game.player[0].points == MAX_POINTS || game.player[1].points == MAX_POINTS) {
        game.player[0].points = game.player[1].points = 0;
        game_over = true;
        music_playing = false;
        player = player_two = 0xff;
        key = RET_TO_MENU;
      }
    }
    ppg_reg_fps();
  }

exit_game:
  close(sock_fd);
  ppg_freeup_game(&game);
  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
  return true;
}
