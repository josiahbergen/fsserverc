#ifndef SERVER_H
#define SERVER_H

int handlepacket(int cfd, char *recvbuf, int recvbytes);
void sendpacket(int cfd, char *sendbuf, int sendbytes);
void printbuffer(char *buffer, int size);

#endif
