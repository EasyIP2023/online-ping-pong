#include <common.h>
#include <netpong.h>
#include <getopt.h>

static void help_message() {
  fprintf(stdout, "Usage: online-ping-pong [options]\n");
  fprintf(stdout, "Example: online-ping-pong --port 5001 --server\n");
  fprintf(stdout, "Options:\n");
  fprintf(stdout, "\t-s, --server\t\t\t Specify that you want the server to start\n");
  fprintf(stdout, "\t-c, --client\t\t\t Specify that you want the client to start\n");
  fprintf(stdout, "\t-p, --port <port>\t\t Specify what port you want to service on/connect to\n");
  fprintf(stdout, "\t-i, --ip <Ip Address>\t\t Specify the ip address you want to connect to\n");
  fprintf(stdout, "\t-h, --help\t\t\t Show this message\n");
}

int main(int argc, char **argv) {
  uint32_t c = 0;
  int8_t track = 0;
  uint16_t port = 0;
  char opts[2][15];

  while (1) {
    int option_index = 0;

    static struct option long_options[] = {
      {"help",   no_argument,       0,  0  },
      {"server", no_argument,       0,  0  },
      {"client", no_argument,       0,  0  },
      {"port",   required_argument, 0,  0  },
      {"ip",     required_argument, 0,  0  },
      {0,        0,                 0,  0  }
    };

    c = getopt_long(argc, argv, "p:i:hsc", long_options, &option_index);
    if (c == UINT32_MAX) goto exit_game;
    track++;

    switch (c) {
      case 0:
        if (!strcmp(long_options[option_index].name, "help")) { help_message(); goto exit_game; }
        if (!strcmp(long_options[option_index].name, "ip")) memcpy(opts[0], optarg, sizeof(opts[0]));
        if (!strcmp(long_options[option_index].name, "port")) memcpy(opts[1], optarg, sizeof(opts[1]));

        /* Specify if it's a client or server last as you need to retrieve the other flags before executing */
        if (!strcmp(long_options[option_index].name, "server")) {
          port = atoi(opts[1]);
          if (port) {
            if (!start_server(port)) goto exit_game;
          } else {
            ppg_log_me(PPG_DANGER, "[x] Usage Example: online-ping-pong --port 5001 --server \n");
            ppg_log_me(PPG_DANGER, "[x] Be sure to specify whether it's a client or server last\n");
            exit(0);
          }
        }

        /* Specify if it's a client or server last as you need to retrieve the other flags before executing */
        if (!strcmp(long_options[option_index].name, "client")) {
          uint16_t port = atoi(opts[1]);
          if (opts[0] && port) {
            if (!start_client(opts[0], port)) goto exit_game;
          } else {
            ppg_log_me(PPG_DANGER, "[x] Usage Example: online-ping-pong --ip 8.8.8.8 --port 5001 --client");
            ppg_log_me(PPG_DANGER, "[x] Be sure to specify whether it's a client or server last");
            exit(0);
          }
        }
        break;
      case 1: break;
      case 104: help_message(); break;
      case 105: memcpy(opts[0], optarg, sizeof(opts[0])); break;
      case 112: memcpy(opts[1], optarg, sizeof(opts[1])); break;
      case 115: /* Specify if it's a client or server last as you need to retrieve the other flags before executing */
        if ((port = atoi(opts[1]))) {
          if (!start_server(port)) goto exit_game;
        } else {
          ppg_log_me(PPG_DANGER, "[x] Usage Example: online-ping-pong -p 5001 -s");
          ppg_log_me(PPG_DANGER, "[x] Be sure to specify whether it's a client or server last");
          exit(0);
        }
        break;
      case 99: /* Specify if it's a client or server last as you need to retrieve the other flags before executing */
        if (opts[0] && (port = atoi(opts[1]))) {
          if (!start_client(opts[0], port)) goto exit_game;
        } else {
          ppg_log_me(PPG_DANGER, "[x] usage example: Example: online-ping-pong -i 127.0.0.1 -p 5001 -c");
          ppg_log_me(PPG_DANGER, "[x] Be sure to specify whether it's a client or server last");
          exit(0);
        }
        break;
      default: fprintf(stdout, "[x] getopt returned character code 0%o ??\n", c); break;
    }
  }

  if (optind < argc) {
    fprintf(stdout, "[x] non-option ARGV-elements: ");
    while (optind < argc)
      fprintf(stdout, "[x] %s ", argv[optind++]);
    fprintf(stdout, "[x] Type online-ping-pong --help for help");
  }

exit_game:
  if (c == UINT32_MAX && track == 0) help_message();
  return EXIT_SUCCESS;
}
