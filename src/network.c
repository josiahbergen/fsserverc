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

//void removeplayer(player **head, int cfd) {
  // find the player to remove, if not found return 1
  // if the player to remove is the head, set the head to the next player
  // else
  // link the player before the removed player to the player after it
  // free the player return 0
  // kill self
//}

void listplayers(player *head) {

  printf(GREEN "Connected players:\n" WHITE);
  player *current = head;
  while (current != NULL) {
    printf("- Player #%d (%s) is in room %d. (%u, %u)\n", current->fd,
           current->name, current->room, current->posx, current->posy);
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

// void broadcastPacket(int *recvbuf, int recvbytes, int *clients, int fdmax) {}
