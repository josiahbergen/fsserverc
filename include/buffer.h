
#ifndef BUFFER_H
#define BUFFER_H
int buffer_write_u8(void *sendbuf, unsigned char *data, int *sendbytes);
unsigned char buffer_read_u8(void *recvbuf, int *recvbytes);

void buffer_send(int cfd, void *sendbuf, int *sendbytes);
void buffer_clear(void *buf, int *pos);

void print_buffer(char *buffer, int size);
#endif
