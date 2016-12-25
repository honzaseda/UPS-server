//
// Created by seda on 27/10/16.
//

#ifndef UPS_SERVER_GAMEROOM_H
#define UPS_SERVER_GAMEROOM_H

#include <vector>
#include <string>
#include <thread>
#include "players.h"
#include "timer.h"

class server;

class players;

class gameRoom {
public:
    gameRoom();

    struct gameInfo {
        int onTurnId;
        int isOver;
        int firstTurned[3] = {-1, 0, 0};
        int secondTurned[3] = {-1, 0, 0};
    };

    struct cards {
        int id;
        bool turned;
    };

    struct rooms {
        int roomId;
        std::string roomName;
        std::vector<players::User> player;
        unsigned long numPlaying;
        unsigned long maxPlaying;
        bool isFull;
        gameInfo info;
        std::vector<cards> roomCards;
    } room;

    int deck[20] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9};

    enum RoomStatus {
        ROOM_WAIT,
        ROOM_READY,
        ROOM_PLAYING
    } roomStatus;

    static std::string getString(RoomStatus status);

    int addPlayer(players::User &player);

    bool removePlayer(players::User &player);

    bool isFull();

    bool playerInOtherRoom(players::User player);

    bool playerAlreadyJoined(players::User player);

    bool setPlayerReady(int playerId, bool ready);

    bool allPlayersReady();

    void createNewGame();

    void turnCard(int playerId, int row, int col);

private:
    std::thread gameThread;

    static void loop(gameRoom *r);

    void shuffleDeck();

    void getRoomWinner(gameRoom *r, server *s);

    void clearRoom(gameRoom *r);

    void sendToPlayers(gameRoom *r, server *s, std::string msg);
};

#endif //UPS_SERVER_GAMEROOM_H
