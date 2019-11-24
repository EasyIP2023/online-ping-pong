#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
* Using unsigned 32 bit integers becuase I want to limit connections
* and limit the amount of stack & heap memory need to be allocated
*/
typedef struct _ppg_client_t {
  uint32_t sock_fd;
  uint32_t state;
  uint32_t game_num; /* Each client has a game they are associated with */
  struct post_t {
    int x, y;
  } pos;
} ppg_client_t;

typedef struct _ppg_ball_t {
  int posX, posY;
} ppg_ball_t;

/* server has a socket fd and one event fd */
typedef struct ppg_server_t {
  uint32_t sock_fd, event_fd;
  struct epoll_event *events;
  uint32_t max_evs;
  struct _games {
    ppg_client_t c1;
    ppg_client_t c2;
    ppg_ball_t ball;
    bool active;
  } *games;
} ppg_server_t;


void ppg_freeup_server(ppg_server_t *server);
bool ppg_create_server(ppg_server_t *server, uint16_t port, uint32_t max_events, uint32_t max_clients);
bool ppg_start_epoll_server(ppg_server_t *server);

#endif