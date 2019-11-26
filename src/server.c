#include <common.h>
#include <net.h>

int main() {
  ppg_server_t *server = ppg_create_server(5001, 64, 1000);
  if (!server) {
    ppg_freeup_server(server);
    return EXIT_FAILURE;
  }

  /* Number of threads is equal to the number of CPU cores I have */
  tpool_t *tp = ppg_tpool_create(3);
  if (!tp) {
    ppg_freeup_server(server);
    return EXIT_FAILURE;
  }

  switch (fork()) {
    case -1:
      ppg_log_me(PPG_DANGER, "[x] fork failed");
      return EXIT_FAILURE;
    case 0:
      for (uint32_t i = 0; i < 16; i++) {
        ppg_connect_client("127.0.0.1", 5001);
        // usleep(10000);
      }
      break;
    default:
      while(ppg_epoll_server(server, tp)) {
        ppg_tpool_wait(tp);
      }
      break;
  }

  ppg_freeup_tpool(tp);
  ppg_freeup_server(server);
  return EXIT_SUCCESS;
}
