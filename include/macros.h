#ifndef MACROS_H
#define MACROS_H

#define TRUE 1
#define FALSE 0

#define EXIT_SUCCESS 0
#define EXIT_ERROR 1

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
    if (!errno) {                                                              \
      printf("\033[1;33mWarning (non-fatal): " WHITE char ".\n");              \
    } else {                                                                   \
    printf("\033[1;33mWarning (non-fatal): " WHITE char ": %s.\n",             \
           strerror(errno));                                                   \
    }                                                                          \
  }

#define NET_PING 0
#define NET_PLAYER_ESTABLISH 1
#define NET_PLAYER_CONNECT 2
#define NET_PLAYER_JOINED 3
#define NET_PLAYER_DISCONNECT 4
#define NET_PLAYER_MOVE 5
#define NET_PLAYER_CHAT 6
#define NET_PLAYER_CHANGE_ROOM 7

#define TYPE_INT 4
#define TYPE_LONG 8
#define TYPE_CHAR 1
#define TYPE_STRING 0

#endif
