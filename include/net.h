#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct _ppg_server_t ppg_server_t;

ppg_server_t *ppg_create_server(uint16_t port, uint32_t max_events, uint32_t max_clients);
void ppg_freeup_server(ppg_server_t *server);
bool ppg_start_epoll_server(ppg_server_t *server);

#endif
