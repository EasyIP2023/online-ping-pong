#include <common.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <net.h>

/**
* EPOLLIN: Associate a file descriptor for read() operations
* EPOLLOUT: Asociate a file descriptor for write() operations
* EPOLLET: Put a file descriptor into edge-triggered mode
*/
static uint32_t common_eflags = EPOLLIN | EPOLLOUT | EPOLLET;

/**
* A single client can have a socket file descriptor (sock_fd)
* A state to help determine what to do with client (state)
* A game they are associated with
* and x, y position cordinate data
*/
struct ppg_client_t {
  uint32_t sock_fd;
  uint32_t state;
  uint32_t game_num;
  int posX, posY;
};

/* Ball has an x y postion cordinate data */
struct ppg_ball_t {
  int posX, posY;
};

/**
* (sock_fd): Server has a socket fd
* (event_fd): There is only one epoll event loop so only one event is need
* (events): There can be multiple multiple FD's that a single epoll loop needs to cycle through
* (max_evs): Keep track of max number of events (FD's) server can handle
* (games_running): Keeps track of number of games currently running
* (struct games): A server can have multiple games
*     (c1, c2): How the server keeps track of client data in a single game
*     (ball): There is a single ball per game, This keeps track of postion data
*     (active): Check to see if a game is active or not
*/
struct _ppg_server_t {
  uint32_t sock_fd, event_fd;
  struct epoll_event *events;
  uint32_t max_evs;
  uint32_t games_running;
  struct _games {
    struct ppg_client_t c1;
    struct ppg_client_t c2;
    struct ppg_ball_t ball;
    bool active;
  } *games;
};

/* representing states with uint32_t 2^32 */
typedef enum _client_states {
  NEW              = 0x00000001,
  ESTABLISHED      = 0x00000002,
  TERMINATE        = 0x00000003,
  MAX_CLIENT_STATE = 0xffffffff
} client_states;

static void init_values(ppg_server_t *server) {
  server->sock_fd = 0;
  server->event_fd = 0;
  server->events = NULL;
  server->max_evs = 0;
  server->games_running = 0;
  server->games = NULL;
}

/**
* I decided to pass the address of the file descriptor
* to ensure that the it gets set into non-blocking mode.
* If I am not mistaken one doesn't need to do that, just
* needs to pass the file descriptor.
*/
static bool make_fd_non_blocking(uint32_t *fd) {
  uint32_t flags;

  if ((flags = fcntl(*fd, F_GETFD, 0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] fcntl(F_GETFD) failed!");
    return false;
  }

  /* Set fd into non-blocking */
  if ((flags = fcntl(*fd, F_SETFD, flags | SOCK_NONBLOCK)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] fcntl(F_SETFD) failed!");
    return false;
  }

  return true;
}

static void init_epoll_values(struct epoll_event *event) {
  event->events = 0;
  event->data.ptr = NULL;
  event->data.fd = 0;
  event->data.u32 = 0;
  event->data.u64 = 0;
}

/* fd is a pointer for the same reason as above in make_fd_non_blocking */
static bool ec_call(ppg_server_t *server, uint32_t *fd, uint32_t events) {
  struct epoll_event event;
  init_epoll_values(&event);
  event.events = events;
  event.data.fd = *fd;
  if (epoll_ctl(server->event_fd, EPOLL_CTL_ADD, *fd, &event) == -1) {
    ppg_log_me(PPG_DANGER, "[x] epoll_ctl failed");
    return true;
  }

  return false;
}

static bool handle_client(ppg_server_t *server, uint32_t cur_game) {
  if (server->games[cur_game].c1.state == NEW) {
    /* Make Client Sock non-blocking */
  	if (!make_fd_non_blocking(&server->games[cur_game].c1.sock_fd)) {
  		ppg_log_me(PPG_DANGER, "[x] make_fd_non_blocking");
  		return true;
  	}

    server->games[cur_game].c1.state = ESTABLISHED;
    server->games[cur_game].c1.game_num = cur_game;
    /* Add client FD to epoll event loop */
    return ec_call(server, &server->games[cur_game].c1.sock_fd, common_eflags);
  }
  if (server->games[cur_game].c2.state == NEW) {
    /* Make Client Sock non-blocking */
    if (!make_fd_non_blocking(&server->games[cur_game].c2.sock_fd)) {
      ppg_log_me(PPG_DANGER, "[x] make_fd_non_blocking");
    	return true;
    }

    server->games[cur_game].c2.state = ESTABLISHED;
    server->games[cur_game].c2.game_num = cur_game;
    /* Add client FD to epoll event loop */
    return ec_call(server, &server->games[cur_game].c2.sock_fd, common_eflags);
  }

  return false;
}

static bool accept_client(ppg_server_t *server, uint32_t cur_game) {
  struct sockaddr_in cl;
  uint32_t c, c_sock;

  c = sizeof(struct sockaddr_in);

  /**
  * I decided not to have and infinite loop here for performance reasons
  * This is okay for this particular application
  */
  c_sock = accept(server->sock_fd, (struct sockaddr *) &cl, (socklen_t *) &c);
  if (c_sock != UINT32_MAX) {
    if (server->games[cur_game].c1.state != NEW) {
      server->games[cur_game].c1.sock_fd = c_sock;
      server->games[cur_game].c1.state = NEW;
    }
    if (server->games[cur_game].c2.state != NEW) {
      server->games[cur_game].c2.sock_fd = c_sock;
      server->games[cur_game].c2.state = NEW;
    }
    return handle_client(server, cur_game);
  }

  return false;
}

void ppg_freeup_server(ppg_server_t *server) {
  if (server->event_fd)
    close(server->event_fd);
  if (server->sock_fd)
    close(server->sock_fd);
  if (server->events)
    free(server->events);
  if (server->games)
    free(server->games);
  init_values(server);
  FREE(server);
}

ppg_server_t *ppg_create_server(uint16_t port, uint32_t max_events, uint32_t max_clients) {
  ppg_server_t *server = (ppg_server_t *) calloc(sizeof(ppg_server_t), sizeof(ppg_server_t));
  if (!server) {
    ppg_log_me(PPG_DANGER, "[x] calloc ppg_server_t *server failed");
    return NULL;
  }

  init_values(server);

  /* Configure settings in address struct */
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = (uint32_t) 0x00000000;
  if (!memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero))) {
    ppg_log_me(PPG_DANGER, "[x] memset call failed");
    return NULL;
  }

  /* Create TCP socket server */
  if ((server->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] Could not create socket");
    return NULL;
  }

  ppg_log_me(PPG_SUCCESS, "Server socket fd %u successfully created", server->sock_fd);

  /* Allows for immediate reuse of a port */
  int i = 1;
  if (setsockopt(server->sock_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(uint32_t)) == -1) {
    ppg_log_me(PPG_DANGER, "[x] setsockopt call failed");
    return NULL;
  }

  /* Bind socket with address struct*/
  if (bind(server->sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    ppg_log_me(PPG_DANGER, "[x] bind failed");
    return NULL;
  }

  ppg_log_me(PPG_SUCCESS, "Server socket fd %u successfully binded", server->sock_fd);

  /* Make server file descriptor non blocking (so unix commands like read won't block) */
  if (!make_fd_non_blocking(&server->sock_fd)) return NULL;

  ppg_log_me(PPG_SUCCESS, "Server socket fd %u is in non-blocking mode", server->sock_fd);

  /* Listen in on server FD, starts server on a port */
  if (listen(server->sock_fd, max_clients) == -1) {
    ppg_log_me(PPG_DANGER, "[x] listen() failed");
    return NULL;
  }

  ppg_log_me(PPG_SUCCESS, "Now listening for connections on %u", port);

  /* Can have 2 clients per game */
  server->games = (struct _games *) calloc(sizeof(struct _games), (max_clients/2) * sizeof(struct _games));
  if (!server->games) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct _games *games failed");
    return NULL;
  }

  /**
  * Create epoll event file descriptor
  * This FD should help regulate other FD's
  * that get added to it
  */
  if ((server->event_fd = epoll_create1(0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] epoll_create1(0) failed");
    return NULL;
  }

  ppg_log_me(PPG_SUCCESS, "Epoll create succeded");

  if (ec_call(server, &server->sock_fd, common_eflags)) return false;

  server->max_evs = max_events * max_clients;
  server->events = (struct epoll_event *) calloc(sizeof(struct epoll_event), server->max_evs * sizeof(struct epoll_event));
  if (!server->events) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct epoll_event *events failed");
    return NULL;
  }

  return server;
}

bool ppg_start_epoll_server(ppg_server_t *server) {
  bool ret = false, c_state = false;
  uint32_t event_nums = 0, game = 0;

  /* Retrieve number of file descriptors in wait-list */
  event_nums = epoll_wait(server->event_fd, server->events, server->max_evs, -1);
  if (event_nums == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] epoll_wait command failed");
    return ret;
  }

  /* check if two client fd's have established a connection */
  for (uint32_t i = 0; i < event_nums; i++) {
    c_state = server->games[game].c1.state == ESTABLISHED &&
              server->games[game].c2.state == ESTABLISHED;
    if (server->events[i].data.fd == (int) server->sock_fd) {
      ret = accept_client(server, game);
      /* if two clients have established a connection set the game activity to true */
      if (c_state) server->games[game].active = true;
      server->games_running += (c_state) ? 1 : 0;
    }
  }

  return ret;
}
