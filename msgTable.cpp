//
// Created by seda on 22/10/16.
//
#include "msgTable.h"

using namespace std;

msgtable::msgTypes msgtable::getType(string msg){
    if(!msg.compare("C_LOGIN")){
        return msgtable::C_LOGIN;
    }
    else if(!msg.compare("C_LOGOUT")){
        return msgtable::C_LOGOUT;
    }
    else if(!msg.compare("C_GET_TABLE")){
        return msgtable::C_GET_TABLE;
    }
    else if(!msg.compare("C_JOIN_ROOM")){
        return msgtable::C_JOIN_ROOM;
    }
    else if(!msg.compare("C_LEAVE_ROOM")){
        return msgtable::C_LEAVE_ROOM;
    }
    else if(!msg.compare("C_ROW_UPDATE")){
        return msgtable::C_ROW_UPDATE;
    }
    else if(!msg.compare("C_ROOM_INFO")){
        return msgtable::C_ROOM_INFO;
    }
    else if(!msg.compare("C_ROOM_USERS")){
        return msgtable::C_ROOM_USERS;
    }
    else if(!msg.compare("C_USR_READY")){
        return msgtable::C_USR_READY;
    }
    else if(!msg.compare("C_USR_NREADY")){
        return msgtable::C_USR_NREADY;
    }
    else if(!msg.compare("C_CHAT")){
        return msgtable::C_CHAT;
    }
    else{
        return msgtable::NO_CODE;
    }
}
