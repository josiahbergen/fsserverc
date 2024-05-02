#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/select.h>

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
    printf("\033[1;31m" char WHITE ": %s.\n", strerror(errno));                \
    return EXIT_ERROR;                                                         \
  }

#define WARN(char)                                                             \
  {                                                                            \
    printf("\033[1;33mWarning: " WHITE char " (non-fatal): %s.\n",             \
           strerror(errno));                                                   \
    return EXIT_ERROR;                                                         \
  }

#define EXIT_SUCCESS 0;
#define EXIT_ERROR 1;

void printBuffer(char buf[], int length) {
  printf(" [" GRAY);
  for (int i = 0; i < length; ++i)
    printf(" %02x", (int)buf[i]);
  printf(WHITE " ]\n");
}

int server(unsigned short port) {

  fd_set main;     // master file descriptor list
  fd_set read_fds; // temp file descriptor list for select()
  int fdmax;       // maximum file descriptor number
  int cfd;         // newly accept()ed socket descriptor

  FD_ZERO(&main); // clear the master and temp sets
  FD_ZERO(&read_fds);

  char recvbuf[1024];
  memset((char *)&recvbuf, 0, sizeof(recvbuf));
  int recvbytes;

  // create server's listening file descriptor
  const int listener = socket(PF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    ERROR("Socket error");
    return EXIT_ERROR;
  }

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
  if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
    ERROR("Bind error");
  }

  // listen on the port
  if (listen(listener, 10)) { // 10 is the maximum pending connections
    ERROR("Listen error");
  }

  // add the listener to the master set
  FD_SET(listener, &main);

  // keep track of the biggest file descriptor
  fdmax = listener; // so far, it's this one

  // setup complete!

  printf(BGREEN "Server is running! " WHITE "( " GRAY "%s:%d" WHITE " )\n",
         inet_ntoa(addr.sin_addr), (int)ntohs(addr.sin_port));

  printf(WHITE "Waiting for new connections...\n");

  while (1) {

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
          char obuffer[1] = {0x01};

          printf("Buffer contents:");
          printBuffer(obuffer, 1);

          int bytes_sent = send(cfd, (void *)&obuffer, sizeof(obuffer), 0);
          if (bytes_sent == -1) {
            WARN("Send error");
          }
          printf("%d byte(s) sent. (%lu expected.)\n", bytes_sent,
                 sizeof(obuffer));

        } else {

          // data from an existing connection

          recvbytes = recv(i, recvbuf, sizeof(recvbuf), 0);

          if (recvbytes == -1) {
            WARN("Recv error");
          } else if (recvbytes == 0) {
            printf(YELLOW "Client %u has disconnnected." WHITE "\n", i);
            FD_CLR(i, &main);
            close(i);
          } else {
            // print the hex contents of the buffer
            printf("%d byte(s) received from client %u. (1024 max)\n",
                   recvbytes, i);
            printf("Received data:");
            printBuffer(recvbuf, recvbytes);
            // reset the buffer
            memset((char *)&recvbuf, 0, sizeof(recvbuf));
          }
        }
      }
    }
  }

  close(listener);
  return EXIT_SUCCESS;
}
