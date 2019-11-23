#include <common.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <net.h>

typedef enum _client_states {
  NEW = 0,
  ESTABLISHED = 1,
  TERMINATED = 2
} client_states;

static void init_values(struct ppg_server_t *server) {
  server->event_fd = 0;
  server->sock_fd = 0;
  server->ev.events = 0;
  server->ev.data.ptr = NULL;
  server->ev.data.fd = 0;
  server->ev.data.u32 = 0;
  server->ev.data.u64 = 0;
  server->events = NULL;
  server->games = NULL;
}

static bool make_non_blocking(int fd) {
  int flags;

  if ((flags = fcntl(fd, F_GETFD, 0)) < 0) {
    ppg_log_me(PPG_DANGER, "[x] fcntl(F_GETFD) failed!");
    return false;
  }

  /* Set fd into non-blocking */
  if (fcntl(fd, F_SETFD, flags | SOCK_NONBLOCK) < 0) {
    ppg_log_me(PPG_DANGER, "[x] fcntl(F_SETFD) failed!");
    return false;
  }

  return true;
}

static bool accept_client(ppg_server_t *server, uint32_t cur_game) {
  struct sockaddr_in cl;
	int c, c_sock;

  c = sizeof(struct sockaddr_in);

  c_sock = accept(server->sock_fd, (struct sockaddr *) &cl, (socklen_t *) &c);
  if (c_sock != -1) {
    if (server->games[cur_game].c1.state == NEW) {
      server->games[cur_game].c1.sock_fd = c_sock;
      server->games[cur_game].c1.state = ESTABLISHED;
    }
    if (server->games[cur_game].c2.state == NEW) {
      server->games[cur_game].c2.sock_fd = c_sock;
      server->games[cur_game].c2.state = ESTABLISHED;
    }
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

  /* Create UDP socket server */
  if ((server->sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] Could not create socket");
    return false;
  }

  /* Configure settings in address struct */
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
  if (!memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero))) {
    ppg_log_me(PPG_DANGER, "[x] memset call failed");
    return false;
  }

  /* Allows for immediate reuse of a port */
  if (setsockopt(server->sock_fd, SOL_SOCKET, SO_REUSEADDR, NULL, sizeof(uint32_t)) == -1) {
    ppg_log_me(PPG_DANGER, "[x] setsockopt call failed");
    return false;
  }

  /* Bind socket with address struct*/
	if (bind(server->sock_fd, (struct sockaddr *) server, sizeof(server)) == -1) {
    ppg_log_me(PPG_DANGER, "[x] bind failed");
    return false;
  }

  /* Make server file descriptor non blocking (so unix commands like read won't block) */
  if (!make_non_blocking(server->sock_fd)) return false;


  /* Listen in on server FD, starts server on a port */
  if (listen(server->sock_fd, max_clients) == -1) {
    ppg_log_me(PPG_DANGER, "[x] listen() failed");
    return false;
  }

  /* Can have 2 clients per game */
  server->games = (struct _games *) calloc(sizeof(struct _games), (max_clients/2) * sizeof(struct _games));
  if (!server->games) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct _games *games failed");
    return false;
  }

  /**
  * Create epoll event file descriptor
  * This FD should help regulate other FD's
  */
  if ((server->event_fd = epoll_create1(0)) == UINT32_MAX) {
    ppg_log_me(PPG_DANGER, "[x] epoll_create1(0) failed");
    return false;
  }

  /**
  * Add Server File Descriptor to epoll event file descriptor
  * EPOLLIN: Associate a file descriptor for read() operations
  * EPOLLOUT: Asociate a file descriptor for write() operations
  * EPOLLET: Put a file descriptor into edge-triggered mode
  */
  server->ev.data.fd = server->sock_fd;
	server->ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	if (epoll_ctl(server->event_fd, EPOLL_CTL_ADD, server->sock_fd, &server->ev) < 0) {
    ppg_log_me(PPG_DANGER, "[x] epoll_ctl failed");
    return false;
  }

  server->events = (struct epoll_event *) calloc(sizeof(struct epoll_event),
        max_events * max_clients * sizeof(struct epoll_event));
  if (!server->events) {
    ppg_log_me(PPG_DANGER, "[x] calloc struct epoll_event *events failed");
    return false;
  }

  return true;
}

bool ppg_start_epoll_server(ppg_server_t *server) {
  accept_client(server, 0);
  return true;
}
