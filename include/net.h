#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <tpool.h>
#include <net-types.h>

ppg_server_t *ppg_create_server(uint16_t port, uint32_t max_events, uint32_t max_clients);
void ppg_freeup_server(ppg_server_t *server);
bool ppg_epoll_server(ppg_server_t *server, tpool_t *tp);
uint32_t ppg_connect_client(const char *ip_addr, uint16_t port);

/* in src/pong/ball.c, ball.img is W: 35px H: 35px */
uint32_t ppg_is_out(uint32_t x);
void ppg_ball_move(ppg_ball_t *ball, uint8_t dir);
void ppg_ball_init(ppg_ball_t *ball);
bool ppg_collision(rect_t rec1, rect_t rec2);

#endif
