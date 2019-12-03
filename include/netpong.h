#ifndef NET_PONG_H
#define NET_PONG_H

bool start_server(uint16_t port);
bool start_client(const char *ip_addr, uint16_t port);

#endif
