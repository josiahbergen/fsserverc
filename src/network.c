#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../include/macros.h"
#include "../include/network.h"
#include "../include/buffer.h"

struct packet {
  int type;
  int size;
  char *data;
};

void printbuffer(char *buffer, int size) {
  printf(GRAY);
  for (int i = 0; i < size; i++) {
      printf(" ");
      for(int j = 7; j >= 0; j--) {
          printf("%d", (buffer[i] >> j) & 1);
      }
  }
  printf(" (");
  for (int i = 0; i < size; i++) {
    printf("%c", buffer[i]);
  }
  printf(")" WHITE "\n");
}

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

  printf("Created player. Username %s, fd %d.\n", newplayer->name,
         newplayer->fd);

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
    // BE FREE, MY CHILD
    free(temp);
  }
}

int handlepacket(int cfd, char *recvbuf, int recvbytes) {
  // print the hex contents of the buffer
  printf("%d byte(s) received from client %u. (1024 max)\n", recvbytes, cfd);
  printf(GRAY "Received data:");
  printbuffer(recvbuf, recvbytes);
  // reset the buffer
  memset((char *)&recvbuf, 0, sizeof(recvbuf));

  return 0;
}

// void broadcastPacket(int *recvbuf, int recvbytes, int *clients, int fdmax) {}
