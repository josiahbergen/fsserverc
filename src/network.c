#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../include/buffer.h"
#include "../include/macros.h"
#include "../include/network.h"

struct packet {
  int type;
  int size;
  char *data;
};

player *createplayer(int cfd, char *name) {
  // allocate memory for a new player
  player *newplayer = (player *)malloc(sizeof(player));

  // set the player's name, file descriptor, position, and other information
  newplayer->fd = cfd;
  newplayer->posx = 0;
  newplayer->posy = 0;
  newplayer->coins = 0;
  newplayer->room = 1;
  strcpy(newplayer->name, name); // unsafe? more like funsafe

  // these are not set by default
  newplayer->next = NULL;
  newplayer->id = -1;

  printf("Created player with file descriptor #%d. Username: %s.\n",
         newplayer->fd, newplayer->name);

  return newplayer;
}

void addplayer(player **head, int cfd, char *name) {

  player *newplayer = createplayer(cfd, name);

  // unite for socialism and it's benefits (use a linked list)

  // if the linked list is empty, set the new player as the head
  if (*head == NULL) {
    *head = newplayer;
    return;
  }

  // iterate through the linked list to find the last player
  player *current = *head;
  while (current->next != NULL) {
    current = current->next;
  }

  // set the next player to the new player
  current->next = newplayer;
}

void listplayers(player *head) {
  player *current = head;
  while (current != NULL) {
    printf("%s is in room %d. (%u, %u)\n", current->name, current->room,
           current->posx, current->posy);
    current = current->next;
  }
}

void freeplayers(player *head) {
  player *current = head;

  while (current != NULL) {
    player *temp = current;
    current = current->next;
    // be free
    free(temp);
  }
}

int handlepacket(int cfd, void *recvbuf, int *recvbytes, player *players) {
  printf("%d byte(s) received from client %u. (1024 max)\n", *recvbytes, cfd);

  u_char msgid = buffer_read_u8(recvbuf, recvbytes);

  switch (msgid) {
  case NET_PING:
    printf("Got NET_PING from #%d: ", cfd);
    break;
  case NET_PLAYER_ESTABLISH:
    printf("Got NET_PLAYER_ESTABLISH from #%d: ", cfd);
    print_buffer(recvbuf, *recvbytes);
    addplayer(&players, cfd, "test");
    listplayers(players);
    break;
  default:
    WARN("Unknown packet ID");
    printf(GRAY "Packet ID is %d.\n" WHITE, msgid);
    return 1;
    break;
  }

  // reset the buffer
  printf(GRAY "Resetting buffer...\n" WHITE);
  memset(recvbuf, 0, sizeof(recvbuf));
  return 0;
}

// void broadcastPacket(int *recvbuf, int recvbytes, int *clients, int fdmax) {}
