#include <common.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <net.h>

static int ERR64 = -1;

/**
* EPOLLIN: Associate a file descriptor for read() operations
* EPOLLOUT: Asociate a file descriptor for write() operations
* EPOLLET: Put a file descriptor into edge-triggered mode
*/
static uint32_t common_eflags = EPOLLIN | EPOLLOUT;

/**
* A single client can have a socket file descriptor (sock_fd)
* A state to help determine what to do with client (state)
* A game they are associated with
* and x, y position cordinate data
*/
struct ppg_client_t {
  uint32_t sock_fd;
  uint32_t state;
  bool playing;
  uint32_t posX, posY;
};

/* Ball has an x y postion cordinate data */
struct ppg_ball_t {
  uint32_t posX, posY;
};

/**
* (sock_fd): Server has a socket fd
* (event_fd): There is only one epoll instance so only one event fd
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
  struct _clients {
    uint32_t gn; /* Game number */
  } *clients;
};

/* representing states with uint32_t 2^32 */
typedef enum _client_states {
  NEW              = 0x00000000,
  ESTABLISHED      = 0x00000001,
  TERMINATE        = 0x00000002,
  MAX_CLIENT_STATE = 0xffffffff
} client_states;

static void init_values(ppg_server_t *server) {
  server->sock_fd = 0;
  server->event_fd = 0;
  server->events = NULL;
  server->max_evs = 0;
  server->games_running = 0;
  server->games = NULL;
  server->clients = NULL;
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
    ppg_log_me(PPG_DANGER, "[x] fcntl(F_GETFD): %s", strerror(errno));
    return false;
  }

  /* Set fd into non-blocking */
  if ((flags = fcntl(*fd, F_SETFD, flags | SOCK_NONBLOCK)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] fcntl(F_SETFD): %s", strerror(errno));
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
  if (epoll_ctl(server->event_fd, EPOLL_CTL_ADD, *fd, &event) == ERR64) {
    ppg_log_me(PPG_DANGER, "[x] epoll_ctl: %s", strerror(errno));
    return false;
  }

  return true;
}

/**
* Checks to see if a client is currently playing a game and if the state of client is established
* if so put client socket file descriptor into non blocking mode and add it's FD
* to the instance of epoll that we are utilizing in this game
*/
static void handle_client(void *serv, void *arg) {
  ppg_server_t *server = (ppg_server_t *) serv;
  uint32_t cur_game = *((uint32_t *) arg);

  if (!server->games[cur_game].c1.playing && server->games[cur_game].c1.state == ESTABLISHED) {
    /* Make Client Sock non-blocking */
  	if (!make_fd_non_blocking(&server->games[cur_game].c1.sock_fd)) return;

    server->games[cur_game].c1.playing = true;
    ppg_log_me(PPG_WARNING, "client1 fd %u is now playing and in non-blocking mode", server->games[cur_game].c1.sock_fd);
    /* Add client FD to epoll event loop, once added leave function */
    if (ec_call(server, &server->games[cur_game].c1.sock_fd, common_eflags)) return;
  }
  if (!server->games[cur_game].c2.playing && server->games[cur_game].c2.state == ESTABLISHED) {
    /* Make Client Sock non-blocking */
    if (!make_fd_non_blocking(&server->games[cur_game].c2.sock_fd)) return;

    server->games[cur_game].c2.playing = true;
    server->games[cur_game].active = true;
    server->games_running++;
    ppg_log_me(PPG_WARNING, "client2 fd %u is now playing in non-blocking mode", server->games[cur_game].c2.sock_fd);
    ppg_log_me(PPG_SUCCESS, "game %d is now active", cur_game);
    /* Add client FD to epoll event loop, once added leave function */
    if (ec_call(server, &server->games[cur_game].c2.sock_fd, common_eflags)) return;
  }
}

static bool accept_client(tpool_t *tp, ppg_server_t *server, uint32_t *cur_game) {
  struct sockaddr_in cl;
  uint32_t c = 0, c_sock = 0;

  c = sizeof(struct sockaddr_in);

  c_sock = accept(server->sock_fd, (struct sockaddr *) &cl, (socklen_t *) &c);
  if (c_sock != UINT32_MAX) {
    if (server->games[*cur_game].c1.state == NEW) {
      server->games[*cur_game].c1.sock_fd = c_sock;
      server->games[*cur_game].c1.state = ESTABLISHED;
      server->clients[c_sock].gn = *cur_game;
      ppg_log_me(PPG_INFO, "client1 connected fd: %u", server->games[*cur_game].c1.sock_fd);
      return ppg_tpool_add_work(tp, handle_client, server, cur_game);
    }
    if (server->games[*cur_game].c2.state == NEW) {
      server->games[*cur_game].c2.sock_fd = c_sock;
      server->games[*cur_game].c2.state = ESTABLISHED;
      server->clients[c_sock].gn = *cur_game;
      ppg_log_me(PPG_INFO, "client2 connected fd: %u", server->games[*cur_game].c2.sock_fd);
      return ppg_tpool_add_work(tp, handle_client, server, cur_game);
    }
  }

  return false;
}

static void transfer_data(uint32_t input_fd, uint32_t output_fd) {
	uint32_t read_len = 0;
	char recv_buff[4096];
	if ((read_len = read(input_fd, recv_buff, sizeof(recv_buff))) != UINT32_MAX) {
		if (write(output_fd, recv_buff, read_len) == EOF) {
			ppg_log_me(PPG_DANGER, "[x] write: %s", strerror(errno));
			return;
		}
	} else {
    ppg_log_me(PPG_DANGER, "[x] read: %s", strerror(errno));
    return;
  }
}

static void play_game(void *serv, void *sock) {
  ppg_server_t *server = (ppg_server_t *) serv;
  uint32_t gn = server->clients[*((uint32_t *) sock)].gn;

	switch (server->games[gn].c1.state) {
		case ESTABLISHED:
      transfer_data(server->games[gn].c1.sock_fd, server->games[gn].c2.sock_fd);
      break;
    case TERMINATE:
      ppg_log_me(PPG_DANGER, "closing client1, socket fd: %u", server->games[gn].c1.sock_fd);
      close(server->games[gn].c1.sock_fd);
      break;
		default: break;
	}
  switch (server->games[gn].c2.state) {
		case ESTABLISHED:
      transfer_data(server->games[gn].c2.sock_fd, server->games[gn].c1.sock_fd);
      break;
    case TERMINATE:
      ppg_log_me(PPG_DANGER, "closing client2, socket fd: %u", server->games[gn].c2.sock_fd);
      close(server->games[gn].c2.sock_fd);
      break;
		default: break;
	}

  if (server->games[gn].c1.state == TERMINATE && server->games[gn].c2.state == TERMINATE) {
    server->games[gn].active = false;
    server->games_running--;
  }
}

void ppg_freeup_server(ppg_server_t *server) {
  close(server->event_fd);
  close(server->sock_fd);
  FREE(server->clients);
  FREE(server->events);
  FREE(server->games);
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
  if (!memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero))) {
    ppg_log_me(PPG_DANGER, "[x] memset call failed");
    return NULL;
  }

  /* Create TCP socket server */
  if ((server->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] socket: %s", strerror(errno));
    return NULL;
  }

  ppg_log_me(PPG_SUCCESS, "Server fd %u successfully created", server->sock_fd);

  int i = 1; /* Allows for immediate reuse of a port */
  if (setsockopt(server->sock_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(uint32_t)) == ERR64) {
    ppg_log_me(PPG_DANGER, "[x] setsockopt: %s", strerror(errno));
    return NULL;
  }

  /* Bind socket with address struct */
  if (bind(server->sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == ERR64) {
    ppg_log_me(PPG_DANGER, "[x] bind: %s", strerror(errno));
    return NULL;
  }

  ppg_log_me(PPG_SUCCESS, "Server fd %u successfully binded", server->sock_fd);

  /* Make server file descriptor non blocking (so unix commands like read(2)/write(2) won't block) */
  if (!make_fd_non_blocking(&server->sock_fd)) return NULL;

  ppg_log_me(PPG_INFO, "Server fd %u is in non-blocking mode", server->sock_fd);

  /* Listen in on server FD, starts server on a port */
  if (listen(server->sock_fd, max_clients) == ERR64) {
    ppg_log_me(PPG_DANGER, "[x] listen: %s", strerror(errno));
    return NULL;
  }

  ppg_log_me(PPG_SUCCESS, "Now listening for connections on port %u", port);

  /* Can have 2 clients per game */
  server->games = (struct _games *) calloc(sizeof(struct _games), (max_clients/2) * sizeof(struct _games));
  if (!server->games) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct _games *games failed");
    return NULL;
  }

  /* Create a file descriptor to a new epoll instance */
  if ((server->event_fd = epoll_create1(0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] epoll_create1: %s", strerror(errno));
    return NULL;
  }

  ppg_log_me(PPG_SUCCESS, "epoll_create1(0) succeded");

  server->max_evs = max_events * max_clients;
  server->events = (struct epoll_event *) calloc(sizeof(struct epoll_event), server->max_evs * sizeof(struct epoll_event));
  if (!server->events) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct epoll_event *events failed");
    return NULL;
  }

  server->clients = (struct _clients *) calloc(sizeof(struct _clients), server->max_evs * sizeof(struct _clients));
  if (!server->clients) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct epoll_event *events failed");
    return NULL;
  }

  /* add server fd to epoll wait-list */
  if (!ec_call(server, &server->sock_fd, common_eflags)) return false;
  ppg_log_me(PPG_SUCCESS, "successfully added sever socket fd to epoll wait list");

  return server;
}

bool ppg_epoll_server(ppg_server_t *server, tpool_t *tp) {
  bool ret = false;
  uint32_t event_num = 0;

  /**
  * Wait for events to occur on the epoll instance
  * return value indicates how many members in the events buffer (sever->events)
  * are filled with event data and can be cycled through
  * ERR64: is here to make sure epoll_wait doesn't block
  */
  event_num = epoll_wait(server->event_fd, server->events, server->max_evs, ERR64);
  if (event_num == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] epoll_wait: %s", strerror(errno));
    return ret;
  }

  for (uint32_t i = 0; i < event_num; i++) {
    if (server->events[i].data.fd == (int) server->sock_fd) {
      /* This works for one use case, this if for class not something people can use for fun */
      ret = accept_client(tp, server, &server->games_running);
      if (!ret) return ret;
    } else {
      ALL_UNUSED(play_game);
      // if ((server->events[i].events & EPOLLIN) || (server->events[i].events & EPOLLOUT)) {
      //   ret = ppg_tpool_add_work(tp, play_game, server, &server->events[i].data.fd);
      //   if (!ret) return ret;
      // }
    }
  }

  return ret;
}

uint32_t ppg_connect_client(const char *ip_addr, uint16_t port) {
  uint32_t client_sock = 0;
  struct sockaddr_in server_addr;

  /*Configure settings in address struct*/
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip_addr);
  if (!memset(server_addr.sin_zero, 0, sizeof (server_addr.sin_zero))) {
    ppg_log_me(PPG_DANGER, "[x] memset call failed");
    return UINT32_MAX;
  }

  /* Create TCP socket*/
  if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] socket: %s", strerror(errno));
    return UINT32_MAX;
  }

  if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    ppg_log_me(PPG_DANGER, "[x] connect: %s", strerror(errno));
    close(client_sock);
    return UINT32_MAX;
  }

  return client_sock;
}
