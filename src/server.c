#include <common.h>
#include <net.h>

int main() {
  bool err = false;

  struct ppg_server_t server;
  err = ppg_create_server(&server, 3001, 64, 1000);
  if (!err) {
    ppg_freeup_server(&server);
    return EXIT_FAILURE;
  }

  ppg_freeup_server(&server);
  return EXIT_SUCCESS;
}
