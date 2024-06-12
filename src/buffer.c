
#include "../include/macros.h"
#include <string.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "../include/macros.h"
#include "../include/network.h"


int buffer_write_int(void *sendbuf, int *data, int *sendbytes) {
    if (sendbuf == NULL) {
        WARN("Sendbuf is NULL");
        return -1;
    }
    int size = sizeof(int);
    memmove(sendbuf, data, size);
    *sendbytes += size;
    printf(GRAY "Wrote %d bytes to the send buffer.\n" WHITE, size);
    return size;
}

// int buffer_read(char *buffer, int type, char *data) {
//     return 0;
// }

void buffer_send(int cfd, char *sendbuf, int *sendbytes) {

    int bytes_sent = send(cfd, sendbuf, *sendbytes, 0);

      if (bytes_sent == -1) {
        WARN("Send error");
      }
      printf("%d byte(s) sent. (%d expected.)\n" GRAY "Buffer contents:", bytes_sent,
             *sendbytes);
      printbuffer(sendbuf, *sendbytes);
}
