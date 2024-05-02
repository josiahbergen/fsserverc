
#define NET_PING 0
#define NET_PLAYER_ESTABLISH 1
#define NET_PLAYER_CONNECT 2
#define NET_PLAYER_JOINED 3
#define NET_PLAYER_DISCONNECT 4
#define NET_PLAYER_MOVE 5
#define NET_PLAYER_CHAT 6
#define NET_PLAYER_CHANGE_ROOM 7

struct packet {
  int type;
  int size;
  char *data;
};
