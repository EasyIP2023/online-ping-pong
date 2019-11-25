#include <common.h>
#include <net.h>

int main() {
  ppg_server_t *server = ppg_create_server(3001, 64, 1000);
  if (!server) {
    ppg_freeup_server(server);
    return EXIT_FAILURE;
  }

  ppg_freeup_server(server);
  return EXIT_SUCCESS;
}
