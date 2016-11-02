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
        int roomId;
        bool isReady;
    };

    static players::User getUserById(int id, std::vector<players::User> users);
    static int getIndexById(int id, std::vector<players::User> users);
};

#endif //UPS_SERVER_PLAYERS_H
