//
// Created by seda on 28/10/16.
//

#ifndef UPS_SERVER_PLAYERS_H
#define UPS_SERVER_PLAYERS_H

#include <string>

class players{
public:
    struct User{
        int uId;
        std::string name;
        int score;
        bool isReady;
    };
};

#endif //UPS_SERVER_PLAYERS_H
