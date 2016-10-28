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
    else{
        return msgtable::NO_CODE;
    }
}
