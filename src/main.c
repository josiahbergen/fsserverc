#include <stdio.h>
#include <string.h>

#include "../include/server.h"

int main(int argc, char *argv[]) {

  unsigned short port;

  // check for command line args
  if (argc == 2) {

    sscanf(argv[1], "%hu", &port);
    printf("Starting server on port %d...\n", port);

    if (port < 1024) {
      fprintf(stderr, "Invalid port number >:(\nPlease specify a port number greater than 1024.\n");
      return -1;
    }

  } else {
    printf("No port specified, using a random avalible port.\n");
    port = 0;
  }

  // run the server!
  server(port);
  return 0;
}
