
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

void print_buffer(char *buffer, int *size) {
  printf(GRAY);
  for (int i = 0; i < *size; i++) {
    for (int j = 7; j >= 0; j--) {
      printf("%d", (buffer[i] >> j) & 1);
    }
    printf(" ");
  }
  printf("(");
  for (int i = 0; i < *size; i++) {
    printf("%c", buffer[i]);
  }
  printf(")" WHITE "\n");
}

int buffer_get_pos(int *pos, void *buffer, int *maxbytes) {
  memcpy(pos, buffer + 1024, sizeof(int));
  if (*pos > *maxbytes) {
    return 1;
  }
  return 0;
}

int buffer_set_pos(int *pos, void *buffer) {
  memcpy(buffer + 1024, pos, 4);
  return 0;
}

int buffer_write_u8(unsigned char *data, void *sendbuf, int *sendbytes) {
  if (sendbuf == NULL) {
    WARN("Sendbuf is NULL");
    return -1;
  }
  int size = sizeof(unsigned char);
  memmove(sendbuf + *sendbytes, data, size);
  *sendbytes += size;
  printf(GRAY "Wrote %d byte(s) to the send buffer.\n" WHITE, size);
  return size;
}

unsigned char buffer_read_u8(unsigned char *data, void *recvbuf,
                             int *recvbytes) {

  int pos;
  if (buffer_get_pos(&pos, recvbuf, recvbytes)) {
    ERROR("Buffer overflow");
  }

  memmove(data, recvbuf + pos, sizeof(unsigned char));
  printf(GRAY "Read %lu byte(s) from position %d.\n" WHITE,
         sizeof(unsigned char), pos);

  pos += sizeof(unsigned char);
  buffer_set_pos(&pos, recvbuf);
  return 0;
}

int buffer_read_string(char data[], int maxlen, void *recvbuf, int *recvbytes) {

  int pos;
  int len = 0;
  if (buffer_get_pos(&pos, recvbuf, recvbytes)) {
    ERROR("Buffer overflow");
  }

  while (*(char *)(recvbuf + len + pos) != '\0' && len < maxlen) {
    len++;
  }

  // 'data', here, is a pointer. 'char data[]' decayed to a pointer to its 
  // first element upon being passed to the function... because C

  memmove(data, recvbuf + pos, len);
  printf(GRAY "Read %d byte(s) from position %d.\n" WHITE, len, pos);

  pos += len;
  buffer_set_pos(&pos, recvbuf);
  return 0;
}

void buffer_send(int cfd, void *sendbuf, int *sendbytes) {

  int bytes_sent = send(cfd, sendbuf, *sendbytes, 0);

  if (bytes_sent == -1) {
    WARN("Send error");
  }
  printf("%d byte(s) sent. (%d expected.)\n" GRAY "Buffer contents: ",
         bytes_sent, *sendbytes);

  print_buffer(sendbuf, sendbytes);
  buffer_clear(sendbuf, sendbytes);
}
