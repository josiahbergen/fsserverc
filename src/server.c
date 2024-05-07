#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/select.h>

#include "../include/network.h"

// https://csperkins.org/teaching/2009-2010/networked-systems/lab04.pdf
// https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
// https://www.gta.ufrj.br/ensino/eel878/sockets/index.html
// https://www.csl.mtu.edu/cs4411.ck/www/NOTES/signal/install.html

#define TRUE 1
#define FALSE 0

// terminal colors :)
#define WHITE "\033[0;37m"
#define GREEN "\033[0;32m"
#define BGREEN "\033[1;32m"
#define YELLOW "\033[0;33m"
#define RED "\033[0;31m"
#define GRAY "\033[0;90m"

#define ERROR(char)                                                            \
  {                                                                            \
    printf("\n\033[1;31m" char WHITE ": %s.\n", strerror(errno));              \
    return EXIT_ERROR;                                                         \
  }

#define WARN(char)                                                             \
  {                                                                            \
    printf("\033[1;33mWarning (non-fatal): " WHITE char ": %s.\n",             \
           strerror(errno));                                                   \
  }

#define EXIT_SUCCESS 0;
#define EXIT_ERROR 1;

typedef struct player {
  char name[16];
  int fd;
  int id;
  int room;
  int coins;
  int posx;
  int posy;
  struct player *next;
} player;

int server(unsigned short port, int *stop) {

  fd_set main;     // master file descriptor list
  fd_set read_fds; // temp file descriptor list for select()
  int fdmax;       // maximum file descriptor number
  int cfd;         // newly accept()ed socket descriptor

  FD_ZERO(&main); // clear the master and temp sets
  FD_ZERO(&read_fds);

  char recvbuf[1024];
  int recvbytes;
  memset((char *)&recvbuf, 0, sizeof(recvbuf));

  char sendbuf[1024];
  int sendbytes;
  memset((char *)&sendbuf, 0, sizeof(sendbuf));

  // player list
  player *players = NULL;

  // create server's listening file descriptor
  printf(GRAY "Creating server socket...\n");

  const int listener = socket(PF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    ERROR("Socket error");
    return EXIT_ERROR;
  }

  // server address (will be localhost:port)
  printf(GRAY "Configuring...\n");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htons(INADDR_ANY);
  addr.sin_port = htons(port);
  memset(&(addr.sin_zero), '\0', 8);

  // client address
  // sockaddr_storage is large enough to hold sockaddr_in and sockaddr_in6
  struct sockaddr_storage caddr;
  socklen_t caddr_len = sizeof(caddr);

  // bind the socket
  printf(GRAY "Binding...\n");

  if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
    ERROR("Bind error");
  }

  // listen on the port
  printf(GRAY "Listening...\n");

  if (listen(listener, 10)) { // 10 is the maximum pending connections
    ERROR("Listen error");
  }

  printf(GRAY "Creating file descriptor sets...\n");
  // add the listener to the master set
  FD_SET(listener, &main);

  // keep track of the biggest file descriptor
  fdmax = listener; // so far, it's this one

  // setup complete!
  printf(GRAY "Setup complete!\n\n");

  printf(BGREEN "Server is running! " WHITE "( " GRAY "%s:%d" WHITE " )\n",
         inet_ntoa(addr.sin_addr), (int)ntohs(addr.sin_port));

  printf(WHITE "Waiting for new connections...\n");

  while (*stop == FALSE) {
    read_fds = main; // copy the main set, as select() will modify it

    // wait for a socket to be ready
    if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) == -1) {
      WARN("Select error");
    }

    // check all the sockets in the read_fds set for events
    for (int i = 0; i < FD_SETSIZE; i++) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == listener) {

          // new connection!

          // set their file descriptor
          cfd = accept(listener, (struct sockaddr *)&caddr, &caddr_len);
          if (cfd == -1) {
            WARN("Accept error");
          } else {
            addplayer(&players, cfd, "ted");
            FD_SET(cfd, &main); // add to master set
            if (i > fdmax)
              fdmax = i;
          }

          printf(GREEN "A client has connected!" WHITE " ( " GRAY
                       "%s:%hu, socket #%u" WHITE " )\n",
                 inet_ntoa(((struct sockaddr_in *)&caddr)->sin_addr), port,
                 cfd);

          // send handshake
          printf("Sending handshake data... ");
          sendbuf[1] = 0x01;
          sendbytes = 1;

          sendpacket(cfd, (char *)&sendbuf, sendbytes);

        } else {

          // data from an existing connection
          recvbytes = recv(i, recvbuf, sizeof(recvbuf), 0);

          if (recvbytes == -1) {
            // error
            WARN("Recv error");
          } else if (recvbytes == 0) {
            // client disconnect
            printf(YELLOW "Client %u has disconnnected." WHITE "\n", i);
            FD_CLR(i, &main);
            close(i);
          } else {
            // regular data
            handlepacket(i, (char *)&recvbuf, recvbytes);
          }
        }
      }
    }
  }
  close(listener);
  printf(RED "QUITTING...\n" WHITE "Cleaning up...");
  freeplayers(players);
  return EXIT_SUCCESS;
}
