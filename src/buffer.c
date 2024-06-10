
#include "../include/macros.h"
#include <string.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "../include/macros.h"
#include "../include/network.h"

int size;

int buffer_write(char (*buffer)[1024], int type, char *data, int *sendbytes) {
    switch (type) {
        case TYPE_INT:
            size = sizeof(int);
            memcpy(buffer, &size, sizeof(int));
            memcpy(buffer + sizeof(int), data, size);
            sendbytes += size;
            return size;
        default:
            return 0;
    }
}

// int buffer_read(char *buffer, int type, char *data) {
//     return 0;
// }

void buffer_send(int cfd, char *sendbuf, int sendbytes) {

    int bytes_sent = send(cfd, (void *)sendbuf, sendbytes, 0);

      if (bytes_sent == -1) {
        WARN("Send error");
      }
      printf("%d byte(s) sent. (%d expected.)\n" GRAY "Buffer contents:", bytes_sent,
             sendbytes);
      printbuffer(sendbuf, sendbytes);
}
