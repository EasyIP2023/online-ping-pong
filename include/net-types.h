#ifndef NET_TYPES_H
#define NET_TYPES_H

#include <const.h>

/* representing states with uint32_t 2^32 */
typedef enum _client_states {
  NEW              = 0x00000000,
  ESTABLISHED      = 0x00000001,
  TERMINATE        = 0x00000002,
  MAX_CLIENT_STATE = 0xffffffff
} client_states;

typedef struct _rect_t {
  uint32_t x, y, w, h;
} rect_t;

/**
* (x_vel): ball velocity X
* (y_vel): ball velocity Y
* rect_t(box):
*   (x,y): postion cordinate data
*   (w,h): Width and height of the image
*/
typedef struct _ball_t {
  uint32_t x_vel, y_vel;
  rect_t box;
} ppg_ball_t;

/**
* A single client can have a socket file descriptor (sock_fd)
* A state to help determine what to do with client (state)
* A game they are associated with
* and x, y position cordinate data
*/
typedef struct _ppg_client_t {
  uint32_t sock_fd;
  uint32_t state;
  bool playing;
  uint32_t y_vel;  /* player only has a y velocity */
  uint32_t points;
  rect_t box;
} ppg_client_t;

/**
* (sock_fd): Server has a socket fd
* (event_fd): There is only one epoll instance so only one event fd
* (events): There can be multiple multiple FD's that a single epoll loop needs to cycle through
* (max_evs): Keep track of max number of events (FD's) server can handle
* (games_running): Keeps track of number of games currently running
* (struct games): A server can have multiple games
*     (c1,c2): How the server keeps track of client data in a single game
*     (ball): There is a single ball per game, This keeps track of postion data
*     (active): Check to see if a game is active or not
* (struct clients): Keeps track of what client FD's belong to what game
*/
typedef struct _ppg_server_t {
  uint32_t sock_fd, event_fd;
  struct epoll_event *events;
  uint32_t max_evs;
  uint32_t games_running;
  struct _games {
    ppg_client_t c1;
    ppg_client_t c2;
    ppg_ball_t ball;
    bool active;
  } *games;
  struct _clients {
    uint32_t gn; /* Game number */
  } *clients;
} ppg_server_t;

#endif
