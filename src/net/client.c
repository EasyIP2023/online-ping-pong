#include <common.h>
#include <net.h>

bool ppg_connect_client(const char *ip_addr, uint16_t port) {
  int client_sock, portNum, nBytes;
  struct sockaddr_in server_addr;
  socklen_t addr_size;

  /* Create UDP socket*/
  client_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (client_sock == -1) return false;

  /*Configure settings in address struct*/
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip_addr);
  memset(server_addr.sin_zero, '\0', sizeof (server_addr.sin_zero));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof(server_addr);

  return true;
}
