//
// Created by seda on 27/10/16.
//

#include "gameRoom.h"
#include "server.h"
#include "players.h"

using namespace std;

gameRoom::gameRoom() {
    roomStatus = RoomStatus::ROOM_WAIT;
}

int gameRoom::addPlayer(players::User &player) {
    if(!playerInOtherRoom(player)) {
        if (!isFull()) {
            if (!playerAlreadyJoined(player)) {
                room.player.at(room.numPlaying).uId = player.uId;
                room.player.at(room.numPlaying).name = player.name;
                room.player.at(room.numPlaying).score = player.score;
                room.player.at(room.numPlaying).isReady = false;

                room.numPlaying++;
                if (room.numPlaying == room.maxPlaying) {
                    roomStatus = RoomStatus::ROOM_READY;
                    room.isFull = true;
                }
                return room.roomId;
            }
            else return -1;
        }
        else return -1;
    }
    else return -1;
}

bool gameRoom::isFull() {
    return room.isFull;
}

bool gameRoom::playerInOtherRoom(players::User player){
    if(player.roomId > -1) return true;
    else return false;
}

bool gameRoom::playerAlreadyJoined(players::User player){
    for(int i = 0; i < room.numPlaying; i++){
        if(player.uId == room.player.at(i).uId) return true;
    }
    return false;
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