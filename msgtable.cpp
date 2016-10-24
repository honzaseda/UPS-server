//
// Created by seda on 22/10/16.
//
#include "msgtable.h"

using namespace std;

msgtable::msgTypes msgtable::getType(string msg){
    if(!msg.compare("C_LOGIN")){
        return msgtable::C_LOGIN;
    }
    else if(!msg.compare("C_LOGOUT")){
        return msgtable::C_LOGOUT;
    }
    else if(!msg.compare("C_GET_PLAYERS")){
        return msgtable::C_GET_PLAYERS;
    }
    else{
        return msgtable::NO_CODE;
    }
}
