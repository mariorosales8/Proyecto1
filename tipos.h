#include <map>


enum tipo{ERROR,WARNING,INFO,STATUS,USERS,MESSAGE,PUBLIC_MESSAGE,
          NEW_ROOM,INVITE,JOIN_ROOM,ROOM_USERS,ROOM_MESSAGE,
          LEAVE_ROOM,DISCONNECT,NEW_USER,NES_STATUS,USER_LIST,
          MESSAGE_FROM,PUBLIC_MESSAGE_FROM,JOINED_ROOM,
          ROOM_USER_LIST,ROOM_MESSAGE_FROM,LEFT_ROOM,DISCONNECTED};

map<std::string, tipo> tipos = {{"ERROR",ERROR},{"WARNING",WARNING},
                           {"INFO",INFO},{"STATUS",STATUS},
                           {"USERS",USERS},{"MESSAGE",MESSAGE},
                           {"PUBLIC_MESSAGE",PUBLIC_MESSAGE},
                           {"NEW_ROOM",NEW_ROOM},{"INVITE",INVITE},
                           {"JOIN_ROOM",JOIN_ROOM},{"ROOM_USERS",ROOM_USERS},
                           {"ROOM_MESSAGE",ROOM_MESSAGE},
                           {"LEAVE_ROOM",LEAVE_ROOM},
                           {"DISCONNECT",DISCONNECT},
                           {"NEW_USER",NEW_USER},{"NES_STATUS",NES_STATUS},
                           {"USER_LIST",USER_LIST},
                           {"MESSAGE_FROM",MESSAGE_FROM},
                           {"PUBLIC_MESSAGE_FROM",PUBLIC_MESSAGE_FROM},
                           {"JOINED_ROOM",JOINED_ROOM},
                           {"ROOM_USER_LIST",ROOM_USER_LIST},
                           {"ROOM_MESSAGE_FROM",ROOM_MESSAGE_FROM},
                           {"LEFT_ROOM",LEFT_ROOM},
                           {"DISCONNECTED",DISCONNECTED}};
