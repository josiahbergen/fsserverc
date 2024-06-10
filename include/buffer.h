
#ifndef BUFFER_H
#define BUFFER_H
    int buffer_write(char (*buffer)[1024], int type, char *data, int *sendbytes);
    // int buffer_read(char *buffer, int type, char *data);
    void buffer_send(int cfd, char *sendbuf, int sendbytes);
#endif
