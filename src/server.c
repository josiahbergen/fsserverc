#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/select.h>

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

  char recvbuf[1024];
  char recvbytes;
  memset((char *)&recvbuf, 0, sizeof(recvbuf));

  char sendbuf[1024];
  char sendbytes;
  memset((char *)&sendbuf, 0, sizeof(sendbuf));

  // create server's listening file descriptor
  printf("Creating server socket...\n");

  const int listener = socket(PF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    ERROR("Socket error");
    return EXIT_ERROR;
  }

  // server address (will be localhost:port)
  printf("Configuring...\n");

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
    WARN("Bind error");

    // override option
    printf("Override? [y/n] ");
    char override = getchar();
    if (override == 'y' || override == 'Y') {
      int yes = 1;
      if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)) == -1) {
          ERROR("setsockopt error");
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
          printf("Sending handshake data... ");
          sendbuf[1] = 1;
          sendbytes = 5;

          sendpacket(cfd, (int *)&sendbuf, sendbytes);

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
            handlepacket(i, (int *)&recvbuf, recvbytes);
          }
        }
      }
    }
  }
  close(listener);
  return EXIT_SUCCESS;
}
