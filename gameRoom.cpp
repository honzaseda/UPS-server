//
// Created by seda on 27/10/16.
//

#include <algorithm>
#include <chrono>
#include "gameRoom.h"
#include "server.h"

using namespace std;

gameRoom::gameRoom() {
    roomStatus = RoomStatus::ROOM_WAIT;
    room.info.isOver = false;
    room.roomCards.resize(20);
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

bool gameRoom::removePlayer(players::User &player) {
    for(int i = 0; i < room.numPlaying; i++){
        if(room.player.at(i).uId == player.uId){
            server::consoleOut("Hráč s id " + to_string(player.uId) + " opustil místnost s id " + to_string(room.roomId));
            room.player.at(i).uId = 0;
            room.player.at(i).name = "";
            room.player.at(i).score = 0;
            room.player.at(i).isReady = false;
            room.numPlaying--;
            roomStatus = RoomStatus::ROOM_WAIT;
            room.isFull = false;
            return true;
        }
    }
    return false;
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

bool gameRoom::setPlayerReady(int playerId, bool ready){
    for(int i = 0; i < room.numPlaying; i++){
        if(room.player.at(i).uId == playerId){
            room.player.at(i).isReady = ready;

            return true;
        }
    }
    return false;
}

bool gameRoom::allPlayersReady(){
    int numReady = 0;
    if(room.numPlaying == room.maxPlaying) {
        for (int i = 0; i < room.numPlaying; i++) {
            if(room.player.at(i).isReady) numReady++;
        }
        if(numReady == room.maxPlaying) {
            createNewGame();
            roomStatus = RoomStatus::ROOM_PLAYING;
            return true;
        }
    }
    return false;
}

void gameRoom::createNewGame(){
    server::consoleOut("[Místnost " + to_string(room.roomId) + "] Všichni hráči připraveni, hra se spouští");
    room.info.onTurnId = 0;
    shuffleDeck();
}

void gameRoom::shuffleDeck(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(std::begin(deck), std::end(deck), std::default_random_engine(seed));
    for(int i = 0; i<20; i++) {
        room.roomCards.at(i).id = deck[i];
        room.roomCards.at(i).turned = false;
    }
}

void gameRoom::turnCard(int playerId, int row, int col){
    if((playerId == room.player.at(room.info.onTurnId).uId)){
        if(!room.roomCards.at(row*5 + col).turned){

        }
    }
    else {
        //TODO hráč není na tahu
    }
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