//
// Created by seda on 27/10/16.
//

#ifndef UPS_SERVER_GAMEROOM_H
#define UPS_SERVER_GAMEROOM_H

#include <vector>
#include <string>
#include "players.h"

class server;
class players;

class gameRoom {
public:
    gameRoom();
    struct rooms {
        int roomId;
        std::string roomName;
        std::vector<players::User> player;
        unsigned long numPlaying;
        unsigned long maxPlaying;
        bool isFull;
    } room;

    enum RoomStatus{
        ROOM_WAIT,
        ROOM_READY,
        ROOM_PLAYING
    }roomStatus;

    static std::string getString(RoomStatus status);

    int addPlayer(players::User &player);
    bool removePlayer(players::User &player);

    bool isFull();
    bool playerInOtherRoom(players::User player);
    bool playerAlreadyJoined(players::User player);

    bool setPlayerReady(int playerId, bool ready);
    bool allPlayersReady();
};

#endif //UPS_SERVER_GAMEROOM_H
