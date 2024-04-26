#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

// https://csperkins.org/teaching/2009-2010/networked-systems/lab04.pdf
// https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
// https://www.gta.ufrj.br/ensino/eel878/sockets/index.html

void server(unsigned short port) {

  // create socket file descriptor
  const int fd = socket(PF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("Socket error");
    return;
  }

  // set the address and port to connect to
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htons(INADDR_ANY);
  addr.sin_port = htons(port);
  memset(&(addr.sin_zero), '\0', 8);

  // bind the socket to the address and port specified earlier
  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))) {
    perror("Bind error");
    return;
  }

  // unneeded? gets the address of the socket
  // socklen_t addr_len = sizeof(addr);
  // getsockname(fd, (struct sockaddr *)&addr, &addr_len);

  // read on the port
  printf("Server is running! (%s:%d)\n", inet_ntoa(addr.sin_addr),
         (int)ntohs(addr.sin_port));

  if (listen(fd, 10)) { // 10 is the maximum number of pending connections
    perror("Listen error");
    return;
  }

  // accept incoming connections
  struct sockaddr_storage caddr; // sockaddr_storage is large enough to hold
                                 // both sockaddr_in and sockaddr_in6

  socklen_t caddr_len = sizeof(caddr);
  const int cfd = accept(fd, (struct sockaddr *)&caddr, &caddr_len);

  printf("A client has connected! (%s)\n", inet_ntoa(addr.sin_addr));

  printf("Sending handshake data... ");
  uint8_t obuffer[1] = {1};
  int bytes_sent = send(cfd, (void *)&obuffer, sizeof(obuffer), 0);

  if (bytes_sent == -1) {
    perror("Send error");
    return;
  }

  printf("%d byte(s) sent. (%lu expected.)\n", bytes_sent, sizeof(obuffer));

  // read from client with recv!
  char buf[1024];
  int bytes_reveived = recv(cfd, buf, sizeof(buf), MSG_WAITALL);

  printf("%d byte(s) reveived. (1024 max)\n", bytes_reveived);

  if (bytes_reveived == 0) {
    printf("Client disconnnected.");
  } else if (bytes_reveived == -1) {
    perror("Recv error");
    return;
  }

  // print without a care in the world
  // printf("client says: %s\n", buf);

  // reset the buffer so we can use it later
  memset((char *)&buf, 0, sizeof(buf));

  close(cfd);
  close(fd);
}
