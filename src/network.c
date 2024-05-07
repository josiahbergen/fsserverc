
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define TRUE 1
#define FALSE 0

// terminal colors :)
#define WHITE "\033[0;37m"
#define GREEN "\033[0;32m"
#define BGREEN "\033[1;32m"
#define YELLOW "\033[0;33m"
#define RED "\033[0;31m"
#define GRAY "\033[0;90m"

#define ERROR(char)                                                            \
  {                                                                            \
    printf("\033[1;31m" char WHITE ": %s.\n", strerror(errno));                \
    return EXIT_ERROR;                                                         \
  }

#define WARN(char)                                                             \
  {                                                                            \
    printf("\033[1;33mWarning (non-fatal): " WHITE char ": %s.\n",             \
           strerror(errno));                                                   \
  }

#define NET_PING 0
#define NET_PLAYER_ESTABLISH 1
#define NET_PLAYER_CONNECT 2
#define NET_PLAYER_JOINED 3
#define NET_PLAYER_DISCONNECT 4
#define NET_PLAYER_MOVE 5
#define NET_PLAYER_CHAT 6
#define NET_PLAYER_CHANGE_ROOM 7

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

struct packet {
  int type;
  int size;
  char *data;
};

void printbuffer(char *buffer, int size) {
  for (int i = 0; i < size; i++) {
    printf(" %02x", buffer[i]);
  }
  printf("\n");
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
  strcpy(newplayer->name, name); // unsafe code? more like funsafe code

  // these are not set by default
  newplayer->next = NULL;
  newplayer->id = -1;

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

void sendpacket(int cfd, char *sendbuf, int sendbytes) {
  int bytes_sent = send(cfd, (void *)sendbuf, sendbytes, 0);
  if (bytes_sent == -1) {
    WARN("Send error");
  }
  printf("%d byte(s) sent. (%d expected.)\nBuffer contents:", bytes_sent,
         sendbytes);
  printbuffer(sendbuf, sendbytes);
}

int handlepacket(int cfd, char *recvbuf, int recvbytes) {
  // print the hex contents of the buffer
  printf("%d byte(s) received from client %u. (1024 max)\n", recvbytes, cfd);
  printf("Received data:");
  printbuffer(recvbuf, recvbytes);
  // reset the buffer
  memset((char *)&recvbuf, 0, sizeof(recvbuf));

  return 0;
}

// void broadcastPacket(int *recvbuf, int recvbytes, int *clients, int fdmax) {}
