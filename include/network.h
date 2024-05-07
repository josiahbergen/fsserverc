#ifndef NETWORK_H
#define NETWORK_H

struct player *createplayer(int cfd, char *name);
void addplayer(struct player **head, int cfd, char *name);
void freeplayers(struct player *head);

int handlepacket(int cfd, char *recvbuf, int recvbytes);
void sendpacket(int cfd, char *sendbuf, int sendbytes);
void printbuffer(char *buffer, int size);

#endif
