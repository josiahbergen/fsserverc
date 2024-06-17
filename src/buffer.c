
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
  memset(buf, 0, *pos);
  printf("%d byte(s) cleared.\n" WHITE, *pos);
  *pos = 0;
}

void print_buffer(char *buffer, int size) {
  printf(GRAY);
  for (int i = 0; i < size; i++) {
    for (int j = 7; j >= 0; j--) {
      printf("%d", (buffer[i] >> j) & 1);
    }
    printf(" ");
  }
  printf("(");
  for (int i = 0; i < size; i++) {
    printf("%c", buffer[i]);
  }
  printf(")" WHITE "\n");
}

int buffer_write_u8(void *sendbuf, u_char *data, int *sendbytes) {
  if (sendbuf == NULL) {
    WARN("Sendbuf is NULL");
    return -1;
  }
  int size = sizeof(u_char);
  memmove(sendbuf + *sendbytes, data, size);
  *sendbytes += size;
  printf(GRAY "Wrote %d byte(s) to the send buffer.\n" WHITE, size);
  return size;
}

unsigned char buffer_read_u8(void *recvbuf, int *recvbytes) {
  if (recvbuf == NULL) {
    WARN("Recvbuf is NULL");
    return 0;
  }
  u_char data;
  int pos;
  memcpy(&pos, recvbuf + 1024, sizeof(int));

  if (pos + 1 > *recvbytes) {
    WARN("buffer overflow womp womp (this is really bad)");
    return 0;
  }

  memmove(&data, recvbuf + pos, sizeof(u_char));
  printf(GRAY "Read %lu byte(s) from position %d.\n" WHITE, sizeof(u_char),
         pos);

  pos += 1;
  memcpy(recvbuf + 1024, &pos, 4);
  return data;
}

void buffer_send(int cfd, void *sendbuf, int *sendbytes) {

  int bytes_sent = send(cfd, sendbuf, *sendbytes, 0);

  if (bytes_sent == -1) {
    WARN("Send error");
  }
  printf("%d byte(s) sent. (%d expected.)\n" GRAY "Buffer contents: ",
         bytes_sent, *sendbytes);

  print_buffer(sendbuf, *sendbytes);
  buffer_clear(sendbuf, sendbytes);
}
