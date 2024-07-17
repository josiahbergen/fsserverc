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

  printf("\n\033[1;36m*** FSSERVERC 1.3 ***" WHITE "\n\n");

  // check for command line args
  if (argc == 3) {
    sscanf(argv[1], "%hu", &port);

    if (port < 1024) {
      status = EXIT_ERROR;
      fprintf(stderr, "\033[1;31mError: Invalid port number >:( " GRAY 
              "\nPlease specify a port number greater than 1024.\n" WHITE);
    }

    sscanf(argv[2], "%hu", &verbosity);

    if (verbosity > 2) {
      status = EXIT_ERROR;
      fprintf(stderr, "\033[1;31mError: Invalid verbosity level >:(" GRAY 
              "\nPlease specify a number between 0 and 2 (inclusive).\n" WHITE);
    }

  } else {

    char *flag = "--help";

    if (argc == 2 && strcmp(argv[1], flag) == 0) {
      printf(BGREEN "Usage: server <port> <verbosity>\n\n"
              BWHITE "Port: " WHITE "Network port to run FSServer on. Must be an integer greater than 1024.\n"
              BWHITE "Verbosity: " WHITE "Control the verbosity of FSServer's output. 0 for minimal (warnings + errors only), 1 for normal, 2 for EVERYTHING.\n");
      EXIT(EXIT_SUCCESS);
    } else {
        status = EXIT_ERROR;
        fprintf(stderr, BRED "Error: Invalid number of arguments >:(" WHITE 
            "\n\nType 'server --help' for more info.\n");
    }
  }

  if (status == EXIT_SUCCESS) {
    // set up signal handler
    signal(SIGINT, INThandler);

    // run the server!
    status = server(port, (int *)&stop, players);
  }
  EXIT(status);
}

void INThandler(int sig) {
  signal(sig, SIG_IGN); // not sure what this does
  printf("\n\n" GRAY "Cleaning up...\n" WHITE);
  freeplayers(players);
  EXIT(EXIT_SUCCESS);
}
