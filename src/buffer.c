
#include "../include/macros.h"
#include <string.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "../include/macros.h"
#include "../include/network.h"

void buffer_clear(void *buf, int *pos) {
  printf(GRAY "Clearing buffer... ");
  int size = sizeof(buf);
  memset(buf, 0, size);
  printf("%d byte(s) cleared.\n" WHITE, size);
  *pos = 0;
}

void print_buffer(char *buffer, int size) {
  printf(GRAY);
  for (int i = 0; i < size; i++) {
    printf(" ");
    for (int j = 7; j >= 0; j--) {
      printf("%d", (buffer[i] >> j) & 1);
    }
  }
  printf(" (");
  for (int i = 0; i < size; i++) {
    printf("%c", buffer[i]);
  }
  printf(")" WHITE "\n");
}

int buffer_write_int(void *sendbuf, int *data, int *sendbytes) {
  if (sendbuf == NULL) {
    WARN("Sendbuf is NULL");
    return -1;
  }
  int size = sizeof(int);
  memmove(sendbuf + *sendbytes, data, size);
  *sendbytes += size;
  printf(GRAY "Wrote %d bytes to the send buffer.\n" WHITE, size);
  return size;
}

// int buffer_read(char *buffer, int type, char *data) {
//     return 0;
// }

void buffer_send(int cfd, void *sendbuf, int *sendbytes) {

  int bytes_sent = send(cfd, sendbuf, *sendbytes, 0);

  if (bytes_sent == -1) {
    WARN("Send error");
  }
  printf("%d byte(s) sent. (%d expected.)\n" GRAY "Buffer contents:",
         bytes_sent, *sendbytes);

  print_buffer(sendbuf, *sendbytes);
  buffer_clear(sendbuf, sendbytes);
}
