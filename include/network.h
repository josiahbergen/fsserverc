#ifndef NETWORK_H
#define NETWORK_H

typedef struct player {
  char name[16];
  int fd;
  int id;
  int room;
  int coins;
  int posx;
  int posy;
  struct player *next;
} player;

struct player *createplayer(int cfd, char *name);
void addplayer(struct player **head, int cfd, char *name);
void freeplayers(struct player *head);
void listplayers(struct player *head);

int handlepacket(int cfd, void *recvbuf, int *recvbytes, player *players);

#endif
