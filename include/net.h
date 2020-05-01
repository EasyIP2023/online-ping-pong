#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tpool.h>
#include <net-types.h>

ppg_server_t *ppg_create_server(uint16_t port, uint32_t max_events, uint32_t max_clients);
void ppg_freeup_server(ppg_server_t *server);
bool ppg_epoll_server(ppg_server_t *server, tpool_t *tp);
uint32_t ppg_connect_client(const char *ip_addr, uint16_t port);
bool make_fd_non_blocking(uint32_t fd);

#endif
