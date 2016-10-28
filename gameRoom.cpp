//
// Created by seda on 27/10/16.
//

#include "gameRoom.h"
#include "server.h"

gameRoom::gameRoom() {
    this->roomStatus = RoomStatus::ROOM_WAIT;
}

void gameRoom::addPlayer(players::User player) {
    if (this->isFull()) {
        this->room.player.at(this->room.numPlaying).uId = player.uId;
        this->room.player.at(this->room.numPlaying).name = player.name;
        this->room.player.at(this->room.numPlaying).score = player.score;
        this->room.player.at(this->room.numPlaying).isReady = false;

        this->room.numPlaying++;
        if (this->room.numPlaying == this->room.maxPlaying) {
            this->room.isFull = true;
        }
    }
}

bool gameRoom::isFull() {
    return this->room.isFull;
}
