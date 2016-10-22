//
// Created by seda on 22/10/16.
//
#include "msgtable.h"

using namespace std;

msgtable::msgTypes msgtable::getType(string msg){
    if(!msg.compare("c_login")){
        return msgtable::C_LOGIN;
    }
    else if(!msg.compare("c_logout")){
        return msgtable::C_LOGOUT;
    }
    else{
        return msgtable::NO_CODE;
    }
}
