#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/select.h>

#include "../include/buffer.h"
#include "../include/macros.h"
#include "../include/network.h"

// https://csperkins.org/teaching/2009-2010/networked-systems/lab04.pdf
// https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
// https://www.gta.ufrj.br/ensino/eel878/sockets/index.html
// https://www.csl.mtu.edu/cs4411.ck/www/NOTES/signal/install.html

int server(unsigned short port, int *stop, player *players) {

  fd_set main;     // master file descriptor list
  fd_set read_fds; // temp file descriptor list for select()
  int fdmax;       // maximum file descriptor number
  int cfd;         // newly accept()ed socket descriptor

  // clear the master and temp sets
  FD_ZERO(&main);
  FD_ZERO(&read_fds);

  void *sendbuf = malloc(1024);
  int sendbytes;
  memset(sendbuf, 0, 1024);

  int recvoffset = sizeof(int);
  void *recvbuf = malloc(1024 + recvoffset);
  int recvbytes;
  memset(recvbuf, 0, 1024 + recvoffset);

  // create server's listening file descriptor
  printf("Creating server socket...\n");

  const int listener = socket(PF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    ERROR("Socket error");
    return EXIT_ERROR;
  }

  printf("Configuring...\n");

  // server address (will be localhost:port)
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
  printf("Binding...\n");

  if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
    ERROR("Bind error");

    // override option (not used)
    printf("Override? [y/n] ");
    char override = getchar();
    if (override == 'y' || override == 'Y') {
      int yes = 1;
      if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
          -1) {
        ERROR("Setsockopt error");
      }
    } else {
      return EXIT_ERROR;
    }
  }

  // listen on the port
  printf("Listening...\n");

  if (listen(listener, 10)) { // 10 is the maximum pending connections
    ERROR("Listen error");
  }

  printf("Creating file descriptor sets...\n");
  // add the listener to the master set
  FD_SET(listener, &main);

  // keep track of the biggest file descriptor
  fdmax = listener; // so far, it's this one

  // setup complete!
  printf("Setup complete!\n\n");

  printf(BGREEN "Server is running! " WHITE "( " GRAY "%s:%d" WHITE " )\n",
         inet_ntoa(addr.sin_addr), (int)ntohs(addr.sin_port));

  printf(GRAY "Press CTRL-C to quit.\n\n" WHITE);
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
            FD_SET(cfd, &main); // add to master set
            if (i > fdmax)
              fdmax = i;
          }

          printf(GREEN "A client has connected!" WHITE " ( " GRAY
                       "%s:%hu, socket #%u" WHITE " )\n",
                 inet_ntoa(((struct sockaddr_in *)&caddr)->sin_addr), port,
                 cfd);

          // send handshake
          printf("Sending handshake data...\n");
          unsigned char handskakedata = NET_PLAYER_ESTABLISH;
          buffer_write_u8(&handskakedata, sendbuf, &sendbytes);
          buffer_send(cfd, sendbuf, &sendbytes);

        } else {

          // data from an existing connection
          recvbytes = recv(i, recvbuf, 1024, 0);

          if (recvbytes == -1) {
            WARN("Recv error");

          } else if (recvbytes == 0) {

            // client disconnect
            printf(YELLOW "Client %u has disconnnected." WHITE "\n", i);
            FD_CLR(i, &main);
            close(i);

          } else {

            // regular data!!

            printf("%d byte(s) received from client %u. (1024 max)\n",
                   recvbytes, i);
            printf("Data received: ");
            print_buffer(recvbuf, &recvbytes);

            // get the message ID
            unsigned char msgid;
            if (buffer_read_u8(&msgid, recvbuf, &recvbytes)) {
              ERROR("Buffer failed to read");
            }

            // big fancy switch statement
            switch (msgid) {
            case NET_PING:
              printf("Got NET_PING from #%d.\n", i);
              break;
            case NET_PLAYER_ESTABLISH:
              printf("Got NET_PLAYER_ESTABLISH from #%d.\n", i);
              char name[16];
              if (buffer_read_string(name, 16, recvbuf, &recvbytes)) {
                ERROR("Buffer failed to read");
              }
              addplayer(&players, i, name);
              listplayers(players);
              unsigned char handskakedata = NET_PLAYER_CONNECT;
              buffer_write_u8(&handskakedata, sendbuf, &sendbytes);
              buffer_send(i, sendbuf, &sendbytes);
              break;
            default:
              printf("\033[1;33mWarning (non-fatal): " WHITE
                     "Unknown message ID (%hu).\n",
                     msgid);
              break;
            }

            // reset the buffer
            printf(GRAY "Done data handling. Resetting buffer...\n" WHITE);
            memset(recvbuf, 0, 1024 + recvoffset);
          }
        }
      }
    }
  }
  close(listener);
  return EXIT_SUCCESS;
}
