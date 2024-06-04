#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <signal.h>
#include <stdlib.h>

#include "../include/macros.h"
#include "../include/network.h"
#include "../include/server.h"

// set up signal handler
void INThandler(int);

int stop = 0;

// initialize linked list of players
player *players = NULL;

int main(int argc, char *argv[]) {

  unsigned short port;

  // printf("unsigned short: %lu\n", sizeof(unsigned short));
  // printf("short: %lu\n", sizeof(short));
  // printf("int: %lu\n", sizeof(int));
  // printf("long: %lu\n", sizeof(long));
  // printf("char: %lu\n", sizeof(char)); 

  FILE *file = fopen("title.txt", "r");
  if (file == NULL) {
    WARN("File error");
  }

  char buffer[256];
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    printf("%s", buffer);
  }

  fclose(file);

  // check for command line args
  if (argc == 2) {
    sscanf(argv[1], "%hu", &port);

    if (port < 1024) {
      fprintf(stderr, "\033[1;31mError\033[0;37m: Invalid port number "
                      ">:(\n\033[0;90mPlease specify a "
                      "port number greater than 1024.\033[0;37m\n");
      exit(1);
    }

  } else {
    printf("\033[1;33mWarning\033[0;37m: No port specified, using a random "
           "avalible port.\n");
    port = 0;
  }

  // set up signal handler
  signal(SIGINT, INThandler);

  // run the server!
  int status = server(port, (int *)&stop, players);

  printf("\nFSServer has quit. %s\033[0;37m\n",
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
