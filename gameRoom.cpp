//
// Created by seda on 27/10/16.
//

#include <algorithm>
#include <chrono>
#include <unistd.h>
#include "gameRoom.h"
#include "server.h"


using namespace std;

gameRoom::gameRoom() {
    roomStatus = RoomStatus::ROOM_WAIT;
    room.info.isOver = false;
    room.roomCards.resize(20);
}

/**
 * Přidává nového hráče do místnosti
 * @param player Struktura hráče
 * @return Vrací id přidaného hráče pokud je přidaný úspěšně, jinak vrací -1
 */
int gameRoom::addPlayer(players::User &player) {
    if (!playerInOtherRoom(player)) {
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
            } else return -1;
        } else return -1;
    } else return -1;
}

/**
 * Odstraňuje hráče z místnosti
 * @param player Struktura hráče
 * @return Vrací true pokud byl úspěšně odebrán, false jinak
 */
bool gameRoom::removePlayer(players::User &player) {
    for (int i = 0; i < room.maxPlaying; i++) {
        if (room.player.at(i).uId == player.uId) {
            server::consoleOut(
                    "[Místnost " + to_string(room.roomId) + "] Hráč s id " + to_string(player.uId) +
                    " opustil místnost");
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

/**
 * Zjišťuje, zda je místnost plná
 * @return Stav místnosti
 */
bool gameRoom::isFull() {
    return room.isFull;
}

/**
 * Kontroluje, zda není hráč v jiné místnosti
 * @param player Struktura hráče
 * @return Boolean hodnota ano, ne
 */
bool gameRoom::playerInOtherRoom(players::User player) {
    if (player.roomId > -1) return true;
    else return false;
}

/**
 * Kontroluje, zda je již hráč připojen v místnosti
 * @param player Struktura hráče
 * @return Boolean hodnota ano, ne
 */
bool gameRoom::playerAlreadyJoined(players::User player) {
    for (int i = 0; i < room.numPlaying; i++) {
        if (player.uId == room.player.at(i).uId) return true;
    }
    return false;
}

/**
 * Nastaví stav hráče na připraven/nepřipraven
 * @param playerId Id hráče
 * @param ready Stav hráče
 * @return Úspěšnost operace
 */
bool gameRoom::setPlayerReady(int playerId, bool ready) {
    for (int i = 0; i < room.numPlaying; i++) {
        if (room.player.at(i).uId == playerId) {
            room.player.at(i).isReady = ready;

            return true;
        }
    }
    return false;
}

/**
 * Kontroluje, zda jsou všichni hráči v místnosti připraveni
 * @return Boolean hodnota ano, ne
 */
bool gameRoom::allPlayersReady() {
    int numReady = 0;
    if (room.numPlaying == room.maxPlaying) {
        for (int i = 0; i < room.numPlaying; i++) {
            if (room.player.at(i).isReady) numReady++;
        }
        if (numReady == room.maxPlaying) {
            createNewGame();
            roomStatus = RoomStatus::ROOM_PLAYING;
            return true;
        }
    }
    return false;
}

/**
 * Vytváří novou hru, spouští herní smyčku v novém vlákně
 */
void gameRoom::createNewGame() {
    server::consoleOut("[Místnost " + to_string(room.roomId) + "] Všichni hráči připraveni, hra se spouští");
    shuffleDeck();
    room.info.onTurnId = 0;
    room.info.turnedBackId = 0;

    gameThread = std::thread(loop, this);
    gameThread.detach();
}

/**
 * Funkce pro zamíchání karet na hracím stole místnosti
 */
void gameRoom::shuffleDeck() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(std::begin(deck), std::end(deck), std::default_random_engine(seed));
    for (int i = 0; i < 20; i++) {
        room.roomCards.at(i).id = deck[i];
        room.roomCards.at(i).turned = false;
    }
}

/**
 * Otáčí kartu na herním stole
 * @param playerId Id hráče, který kartu otočil
 * @param row řádek otáčené karty
 * @param col sloupec otáčené karty
 */
void gameRoom::turnCard(int playerId, int row, int col) {
    server *s = new server();
    if ((playerId == room.player.at(room.info.onTurnId).uId)) {
        if (!room.roomCards.at(row * 5 + col).turned) {
            if (room.info.firstTurned[0] < 0) {
                string msg = "S_TURNED:" + to_string(row) + ":" + to_string(col) + ":" +
                             to_string(room.roomCards.at(5 * row + col).id) +
                             "#" += '\n';
                sendToPlayers(this, s, msg);
                room.info.firstTurned[0] = room.roomCards.at(5 * row + col).id;
                room.info.firstTurned[1] = row;
                room.info.firstTurned[2] = col;
            } else if (room.info.secondTurned[0] < 0) {
                if(!((row == room.info.firstTurned[1]) && (col == room.info.firstTurned[2]))) {
                    string msg = "S_TURNED:" + to_string(row) + ":" + to_string(col) + ":" +
                                 to_string(room.roomCards.at(5 * row + col).id) +
                                 "#" += '\n';
                    sendToPlayers(this, s, msg);
                    room.info.secondTurned[0] = room.roomCards.at(5 * row + col).id;
                    room.info.secondTurned[1] = row;
                    room.info.secondTurned[2] = col;
                }
            } else {
            }
        }
    } else {
        string notOnTurn = "S_NON_TURN:" + to_string(!room.info.onTurnId) +
                           "#" += '\n';
        s->sendMsg(playerId, notOnTurn);
    }
}

/**
 * Funkce obstarávající herní smyčku místnosti
 * @param r Ukazatel na místnost
 */
void gameRoom::loop(gameRoom *r) {
    unsigned long turnDuration = 30; //in seconds
    unsigned long turnTimeNotify = 20;
    unsigned long visibleDuration = 3;
    unsigned long turnTimeoutDuration = 20;
    server *s = new server();
    timer turn;
    timer visible;
    timer turnTimeout;
    bool timeMsgSent = false;
    r->room.info.isOver = 0;
    turnTimeout.start();
    while (!r->allTurnedBack(r)) {
        if (turnTimeout.elapsedTime() >= turnTimeoutDuration) {
            r->getRoomWinner(r, s);
            r->clearRoom(r);
            break;
        }
    }
    r->room.info.turnedBackId = 0;
    string msg = "S_ON_TURN:" + to_string(r->room.info.onTurnId) +
                 "#" += '\n';
    s->sendMsg(r->room.player.at(r->room.info.onTurnId).uId, msg);
    while (r->room.info.isOver < r->room.roomCards.size() / 2) {
        turn.start();
        timeMsgSent = false;
        while (r->room.numPlaying == r->room.maxPlaying) {
            if ((turn.elapsedTime() >= turnTimeNotify) && !timeMsgSent){
                s->sendTimeMsg(r, r->room.info.onTurnId);
                timeMsgSent = true;
            }
            if (turn.elapsedTime() >= turnDuration) {
                string timeOut = "S_TIME:" + to_string(r->room.roomId) +
                                 "#" += '\n';
                s->sendMsg(r->room.info.onTurnId, timeOut);
                if(r->room.numPlaying > 0) {
                    r->room.info.onTurnId = (++r->room.info.onTurnId) % r->room.numPlaying;
                }
                else{
                    break;
                }
                string onTurnTime = "S_ON_TURN:" + to_string(r->room.info.onTurnId) +
                                    "#" += '\n';
                r->sendToPlayers(r, s, onTurnTime);
                if (r->room.info.firstTurned[0] != -1) {
                    string turnedOne =
                            "S_TURNBACK:" +
                            to_string(r->room.info.firstTurned[1]) + ":" +
                            to_string(r->room.info.firstTurned[2]) + ":" +
                            to_string(r->room.info.firstTurned[1]) +
                            ":" + to_string(r->room.info.firstTurned[2]) +
                            "#" += '\n';
                    r->sendToPlayers(r, s, turnedOne);
                    turnTimeout.start();
                    while (!r->allTurnedBack(r)) {
                        if (turnTimeout.elapsedTime() >= turnTimeoutDuration) {
                            r->getRoomWinner(r, s);
                            r->clearRoom(r);
                            break;
                        }
                    }
                    r->room.info.turnedBackId = 0;
                    r->room.info.firstTurned[0] = -1;
                }
                break;
            } else {
                if ((r->room.info.firstTurned[0] >= 0) && (r->room.info.secondTurned[0] >= 0)) {
                    if ((r->room.info.firstTurned[0] == r->room.info.secondTurned[0])) {
                        r->room.player.at(r->room.info.onTurnId).score++;
                        s->consoleOut("[Místnost " + to_string(r->room.roomId) + "] Hráč s id " +
                                      to_string(r->room.player.at(r->room.info.onTurnId).uId) + " získal bod");
                        r->room.info.isOver++;
                        visible.start();
                        while (true) {
                            if (visible.elapsedTime() >= visibleDuration) {
                                string scored =
                                        "S_SCORED:" + to_string(r->room.info.onTurnId) + ":" +
                                        to_string(r->room.player.at(r->room.info.onTurnId).score) + ":" +
                                        to_string(r->room.info.firstTurned[1]) + ":" +
                                        to_string(r->room.info.firstTurned[2]) + ":" +
                                        to_string(r->room.info.secondTurned[1]) +
                                        ":" + to_string(r->room.info.secondTurned[2]) +
                                        "#" += '\n';
                                r->sendToPlayers(r, s, scored);
                                break;
                            }
                        }
                        r->room.info.firstTurned[0] = -1;
                        r->room.info.secondTurned[0] = -1;
                        break;
                    } else {
                        visible.start();
                        while (true) {
                            if (visible.elapsedTime() >= visibleDuration) {
                                string notSame =
                                        "S_TURNBACK:" +
                                        to_string(r->room.info.firstTurned[1]) + ":" +
                                        to_string(r->room.info.firstTurned[2]) + ":" +
                                        to_string(r->room.info.secondTurned[1]) +
                                        ":" + to_string(r->room.info.secondTurned[2]) +
                                        "#" += '\n';
                                r->sendToPlayers(r, s, notSame);
                                break;
                            }
                        }
                        turnTimeout.start();
                        while (!r->allTurnedBack(r)) {
                            if (turnTimeout.elapsedTime() >= turnTimeoutDuration) {
                                r->getRoomWinner(r, s);
                                r->clearRoom(r);
                                break;
                            }
                        }
                        r->room.info.turnedBackId = 0;
                        r->room.info.onTurnId = (++r->room.info.onTurnId) % r->room.numPlaying;
                        string onTurn = "S_ON_TURN:" + to_string(r->room.info.onTurnId) +
                                        "#" += '\n';
                        r->sendToPlayers(r, s, onTurn);
                        r->room.info.firstTurned[0] = -1;
                        r->room.info.secondTurned[0] = -1;
                        break;
                    }
                }
            }
        }
    }
    if(r->room.numPlaying == r->room.maxPlaying) {
        r->getRoomWinner(r, s);
    }
    r->clearRoom(r);
    s->consoleOut("[Místnost " + to_string(r->room.roomId) + "] Místnost vyčištěna");
}

/**
 * Inkrementuje počet hráčů, kteří otočili zpět karty odkryté na herním stole
 */
void gameRoom::addTurned() {
    room.info.turnedBackId++;
}

/**
 * Kontroluje, zda všichni hráči otočili zpět odkryté karty
 * @param r Ukazatel na místnost
 * @return Boolean hodnota ano, ne
 */
bool gameRoom::allTurnedBack(gameRoom *r) {
    return (r->room.info.turnedBackId == r->room.numPlaying);
}

/**
 * Vyhodnocuje vítězného hráče hry
 * @param r Ukazatel na místnost
 * @param s Instance serveru
 */
void gameRoom::getRoomWinner(gameRoom *r, server *s) {
    int winnerId = 0;
    int secondId = 0;
    int winningScore = 0;
    for (int i = 0; i < r->room.numPlaying; i++) {
        if (r->room.player.at(i).score > winningScore) {
            winnerId = i;
            winningScore = r->room.player.at(i).score;
        } else if (r->room.player.at(i).score <= winningScore) {
            secondId = i;
        }
    }
    string gameEnd;
    if (r->room.player.at(winnerId).score == r->room.player.at(secondId).score) {
        s->consoleOut("[Místnost " + to_string(r->room.roomId) + "] Konec hry. Remíza hráčů s id " +
                      to_string(r->room.player.at(winnerId).uId) + " a " + to_string(r->room.player.at(secondId).uId) +
                      " se skóre " +
                      to_string(winningScore));
        gameEnd = "S_GAME_END:0:" + to_string(winnerId) + ":" + to_string(secondId) + ":" + to_string(winningScore) +
                  "#" += '\n';
    } else {
        s->consoleOut("[Místnost " + to_string(r->room.roomId) + "] Konec hry. Vyhrál hráč s id " +
                      to_string(r->room.player.at(winnerId).uId) + " se skóre " +
                      to_string(r->room.player.at(winnerId).score));
        gameEnd = "S_GAME_END:1:" + to_string(winnerId) + ":" + to_string(winningScore) +
                  "#" += '\n';
    }
    r->sendToPlayers(r, s, gameEnd);
}

/**
 * Vyčistí herní místnost po dohrání hry
 * @param r Instance místnosti
 */
void gameRoom::clearRoom(gameRoom *r) {
    r->room.isFull = false;
    r->roomStatus = RoomStatus::ROOM_WAIT;
    r->room.info.isOver = r->room.roomCards.size() / 2;
    r->room.info.firstTurned[0] = -1;
    r->room.info.firstTurned[1] = 0;
    r->room.info.firstTurned[2] = 0;
    r->room.info.secondTurned[0] = -1;
    r->room.info.secondTurned[1] = 0;
    r->room.info.secondTurned[2] = 0;
    r->room.info.turnedBackId = 0;
}

/**
 * Pošle zprávu všem hráčům v místnosti
 * @param r Instance místnosti
 * @param s Instance serveru
 * @param msg Posílaná zpráva
 */
void gameRoom::sendToPlayers(gameRoom *r, server *s, string msg) {
    for (int i = 0; i < r->room.maxPlaying; i++) {
        if (r->room.player.at(i).uId != 0) {
            s->sendMsg(r->room.player.at(i).uId, msg);
        }
    }
}

/**
 * Vrací stav místnosti
 * @param status Struktura status
 * @return Stav místnosti
 */
string gameRoom::getString(RoomStatus status) {
    if (status == RoomStatus::ROOM_WAIT) {
        return "ROOM_WAIT";
    } else if (status == RoomStatus::ROOM_READY) {
        return "ROOM_READY";
    } else if (status == RoomStatus::ROOM_PLAYING) {
        return "ROOM_PLAYING";
    }
}