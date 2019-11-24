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
static uint32_t games_running = 0;

typedef enum _client_states {
  NEW = 1,
  ESTABLISHED = 2,
  TERMINATE = 3,
  MAX_CLIENT_STATE = 0xffffffff /* representing states with uint32_t 2^32 */
} client_states;

static void init_values(struct ppg_server_t *server) {
  server->sock_fd = 0;
  server->event_fd = 0;
  server->events = NULL;
  server->max_evs = 0;
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

/* fd is a pointer for the same reason as above in make_fd_non_blocking */
static bool ec_call(ppg_server_t *server, uint32_t *fd, uint32_t events) {
  struct epoll_event event;
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

  /* I decided not to have and infinite loop here for performance reasons */
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
}

bool ppg_create_server(ppg_server_t *server, uint16_t port, uint32_t max_events, uint32_t max_clients) {
  init_values(server);

  /* Configure settings in address struct */
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  if (!memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero))) {
    ppg_log_me(PPG_DANGER, "[x] memset call failed");
    return false;
  }

  /* Create TCP socket server */
  if ((server->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] Could not create socket");
    return false;
  }

  ppg_log_me(PPG_SUCCESS, "Server socket fd %u successfully created", server->sock_fd);

  /* Allows for immediate reuse of a port */
  int i = 1;
  if (setsockopt(server->sock_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(uint32_t)) == -1) {
    ppg_log_me(PPG_DANGER, "[x] setsockopt call failed");
    return false;
  }

  /* Bind socket with address struct*/
  if (bind(server->sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    ppg_log_me(PPG_DANGER, "[x] bind failed");
    return false;
  }

  ppg_log_me(PPG_SUCCESS, "Server socket fd %u successfully binded", server->sock_fd);

  /* Make server file descriptor non blocking (so unix commands like read won't block) */
  if (!make_fd_non_blocking(&server->sock_fd)) return false;

  ppg_log_me(PPG_SUCCESS, "Server socket fd %u is in non-blocking mode", server->sock_fd);

  /* Listen in on server FD, starts server on a port */
  if (listen(server->sock_fd, max_clients) == -1) {
    ppg_log_me(PPG_DANGER, "[x] listen() failed");
    return false;
  }

  ppg_log_me(PPG_SUCCESS, "Now listening for connections on %u", port);

  /* Can have 2 clients per game */
  server->games = (struct _games *) calloc(sizeof(struct _games), (max_clients/2) * sizeof(struct _games));
  if (!server->games) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct _games *games failed");
    return false;
  }

  /**
  * Create epoll event file descriptor
  * This FD should help regulate other FD's
  * that get added to it
  */
  if ((server->event_fd = epoll_create1(0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] epoll_create1(0) failed");
    return false;
  }

  ppg_log_me(PPG_SUCCESS, "Epoll create succeded");

  if (ec_call(server, &server->sock_fd, common_eflags)) return false;

  server->max_evs = max_events * max_clients;
  server->events = (struct epoll_event *) calloc(sizeof(struct epoll_event), server->max_evs * sizeof(struct epoll_event));
  if (!server->events) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct epoll_event *events failed");
    return false;
  }

  return true;
}

bool ppg_start_epoll_server(ppg_server_t *server) {
  bool ret = false;
  uint32_t event_nums = 0;
  /* Retrieve number of file descriptors in wait-list */
  event_nums = epoll_wait(server->event_fd, server->events, server->max_evs, -1);
  if (event_nums == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] epoll_wait command failed");
    return ret;
  }

  /* check if two client fd's have established a connection */
  bool ce_state = server->games[games_running].c1.state == ESTABLISHED &&
                  server->games[games_running].c2.state == ESTABLISHED;
  for (uint32_t i = 0; i < event_nums; i++) {
		if (server->events[i].data.fd == (int) server->sock_fd) {
      ret = accept_client(server, games_running);
      /* if two clients have established a connection set the game activity to true */
      if (ce_state) server->games[games_running].active = true;
      games_running += (ce_state) ? 1 : 0;
    }
  }

  return ret;
}
