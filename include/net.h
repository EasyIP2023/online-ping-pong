#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tpool.h>

typedef struct _ppg_server_t ppg_server_t;

bool ppg_connect_client(const char *ip_addr, uint16_t port);

ppg_server_t *ppg_create_server(uint16_t port, uint32_t max_events, uint32_t max_clients);
void ppg_freeup_server(ppg_server_t *server);
bool ppg_epoll_server(ppg_server_t *server, tpool_t *tp);
bool game_is_active(ppg_server_t *server, uint32_t game);

#endif
