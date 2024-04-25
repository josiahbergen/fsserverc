#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

// https://csperkins.org/teaching/2009-2010/networked-systems/lab04.pdf
// https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
// https://www.gta.ufrj.br/ensino/eel878/sockets/index.html

static void server(unsigned short port) {

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

  // read on the port
  socklen_t addr_len = sizeof(addr);
  // getsockname(fd, (struct sockaddr *)&addr, &addr_len); unneeded?
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

  printf("A client has connected! (cfd %u)\n", cfd);

  printf("Sending handshake data... ");
  char data = "1";
  char *datap = &data;
  int bytes_sent = send(cfd, datap, 1, 0);

  if (bytes_sent == -1) {
    perror("Send error");
    return;
  }

  printf("%d bytes sent. (1 expected.)\n", bytes_sent);

  // read from client with recv!
  char buf[1024];
  int bytes_reveived = recv(cfd, buf, sizeof(buf), MSG_WAITALL);

  printf("%d bytes reveived. (1024 max)", bytes_reveived);

  if (bytes_reveived == 0) {
    printf("Client disconnnected.");
  } else if (bytes_reveived == -1) {
    perror("Recv error");
    return;
  }

  // print without a care in the world
  printf("client says: %s\n", buf);

  // reset the buffer so we can use it later
  memset((char *)&buf, 0, sizeof(buf));

  close(cfd);
  close(fd);
}

int main(int argc, char *argv[]) {

  unsigned short port;

  // check for command line args
  if (argc == 2) {

    sscanf(argv[1], "%hu", &port);
    printf("Starting server on port %d...\n", port);

    if (port < 1024 || port > 65535) {
      fprintf(stderr, "Invalid port number >:(\nPlease specify a port number "
                      "between 1024 and 65535.\n");
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
