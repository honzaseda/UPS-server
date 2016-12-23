//
// Created by seda on 29/10/16.
//

#include <vector>
#include "players.h"

players::User players::getUserById(int id, std::vector<players::User> users) {
    for (int i = 0; i < users.size(); i++) {
        if (users.at(i).uId == id) return users.at(i);
    }
}

int players::getIndexById(int id, std::vector<players::User> users) {
    for (int i = 0; i < users.size(); i++) {
        if (users.at(i).uId == id) return i;
    }
}