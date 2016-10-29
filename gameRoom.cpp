//
// Created by seda on 27/10/16.
//

#include "gameRoom.h"
#include "server.h"

using namespace std;

gameRoom::gameRoom() {
    this->roomStatus = RoomStatus::ROOM_WAIT;
}

void gameRoom::addPlayer(players::User player) {
    if (!this->isFull()) {
        this->room.player.at(this->room.numPlaying).uId = player.uId;
        this->room.player.at(this->room.numPlaying).name = player.name;
        this->room.player.at(this->room.numPlaying).score = player.score;
        this->room.player.at(this->room.numPlaying).isReady = false;

        this->room.numPlaying++;
        if (this->room.numPlaying == this->room.maxPlaying) {
            this->roomStatus = RoomStatus::ROOM_READY;
            this->room.isFull = true;
        }
        cout << "Hráč (" << player.uId << ")" << player.name << " se připojil do místnosti " << this->room.roomId << endl;
    }
}

bool gameRoom::isFull() {
    return this->room.isFull;
}

bool gameRoom::playerAlreadyJoined(players::User player){

}

string gameRoom::getString(RoomStatus status){
    if(status == RoomStatus::ROOM_WAIT){
        return "ROOM_WAIT";
    }
    else if(status == RoomStatus::ROOM_READY){
        return "ROOM_READY";
    }
    else if(status == RoomStatus::ROOM_PLAYING){
        return "ROOM_PLAYING";
    }
}