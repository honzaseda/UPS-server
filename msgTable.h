//
// Created by seda on 22/10/16.
//

#ifndef UPS_SERVER_MSGTABLE_H
#define UPS_SERVER_MSGTABLE_H

#include <string>

class msgtable{
public:
    //enumeration of message types
    enum msgTypes{
        S_LOGGED,
        S_NAME_EXISTS,
        S_SERVER_FULL,
        C_LOGIN,
        C_LOGOUT,
        C_GET_TABLE,
        C_JOIN_ROOM,
        C_LEAVE_ROOM,
        C_ROW_UPDATE,
        C_ROOM_INFO,
        C_USR_READY,
        C_USR_NREADY,
        C_CHAT,
        NO_CODE
    };
    static msgTypes getType(std::string msg);
};

#endif //UPS_SERVER_MSGTABLE_H
