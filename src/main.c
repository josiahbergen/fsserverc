#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <stdlib.h>

#include "../include/macros.h"
#include "../include/network.h"
#include "../include/server.h"

// set up signal handler
void INThandler(int);

int stop = 0;
unsigned short verbosity;

// initialize linked list of players
player *players = NULL;

int main(int argc, char *argv[]) {

  unsigned short port;
  int status = EXIT_SUCCESS;

  // FILE *file = fopen("title.txt", "r");
  // if (file == NULL) {
  //   WARN("File error");
  // }

  // char buffer[256];
  // while (fgets(buffer, sizeof(buffer), file) != NULL) {
  //   printf("%s", buffer);
  // }

  // fclose(file);

  printf("\n\033[1;36mFSSERVERC 0.3 - " WHITE
         "Copyright Josiah Bergen 2024.\n\n");

  // check for command line args
  if (argc == 3) {
    sscanf(argv[1], "%hu", &port);

    if (port < 1024) {
      fprintf(stderr, "\033[1;31mError: Invalid port number "
                      ">:( " GRAY "\nPlease specify a "
                      "port number greater than 1024.\n" WHITE);
      status = EXIT_ERROR;
    }

    sscanf(argv[2], "%hu", &verbosity);

    if (verbosity > 2) {
      fprintf(stderr, "\033[1;31mError: Invalid verbosity level "
                      ">:(" GRAY "\nPlease specify a "
                      "number between 0 and 2 (inclusive).\n" WHITE);
      status = EXIT_ERROR;
    }

  } else {
    fprintf(stderr,
            "\033[1;31mError: Invalid number of arguments "
            ">:(" WHITE "\nUsage: %s <port> <verbosity>\n",
            argv[0]);
    status = EXIT_ERROR;
  }

  if (status == EXIT_SUCCESS) {
    // set up signal handler
    signal(SIGINT, INThandler);

    // run the server!
    status = server(port, (int *)&stop, players);
  }

  printf("\nFSServer has quit. %s\n",
         status == 1 ? "\033[1;31m[EXIT_ERROR]" : "\033[1;32m[EXIT_SUCCESS]");

  exit(status == 1 ? 1 : 0);
}

void INThandler(int sig) {
  signal(sig, SIG_IGN); // not sure what this does
  printf("\n\n" GRAY "Cleaning up...\n" WHITE);
  freeplayers(players);
  printf("FSServer has quit. \033[1;32m[EXIT_SUCCESS]\n" WHITE);
  exit(1);
}
