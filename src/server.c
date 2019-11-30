#include <common.h>
#include <net.h>

#include <netpong.h>

bool start_server(uint16_t port) {
  ppg_server_t *server = ppg_create_server(port, 64, 1000);
  if (!server) {
    ppg_freeup_server(server);
    return false;
  }

  /* Number of threads is equal to the number of CPU cores I have */
  tpool_t *tp = ppg_tpool_create(3);
  if (!tp) {
    ppg_freeup_server(server);
    return false;
  }

  while(ppg_epoll_server(server, tp)) {
    ppg_tpool_wait(tp);
  }

  ppg_freeup_tpool(tp);
  ppg_freeup_server(server);
  return true;
}
