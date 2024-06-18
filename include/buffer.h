
#ifndef BUFFER_H
#define BUFFER_H

int buffer_get_pos(int *pos, void *buffer, int *maxbytes);
int buffer_set_pos(int *pos, void *buffer);

int buffer_write_u8(unsigned char *data, void *sendbuf, int *sendbytes);
int buffer_read_u8(unsigned char *data, void *recvbuf, int *recvbytes);

int buffer_read_string(char data[], int len, void *recvbuf, int *recvbytes);

void buffer_send(int cfd, void *sendbuf, int *sendbytes);
void buffer_clear(void *buf, int *pos);

void print_buffer(char *buffer, int *size);
#endif
