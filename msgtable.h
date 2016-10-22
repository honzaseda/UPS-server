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
        C_LOGIN,
        C_LOGOUT,
        C_GET_TABLE,
        NO_CODE
    };
    static msgTypes getType(std::string msg);
};

#endif //UPS_SERVER_MSGTABLE_H
