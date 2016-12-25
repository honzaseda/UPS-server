//
// Created by Seda on 20. 10. 2016.
//

#include <unistd.h>
#include <stdlib.h>
#include "server.h"
#include "stl.h"
#include "msgTable.h"
#include "gameRoom.h"
#include <time.h>

using namespace std;

const int SERVER_PORT = 44444;

server::server() {
    users.resize((unsigned long) (MAX_CONNECTED));
    serverPort = SERVER_PORT;
}

void server::start() {
    connectedUsers = 0;
    serverFull = false;
    sockfd = -1;

    for (int i = 0; i < MAX_CONNECTED; i++) {
        clientSockets[i] = 0;
    }
    //vytvoření socketu
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0) {
        consoleOut("Chyba při vytvoření socketu");
        exit(1);
    }


    int optionVal = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optionVal, sizeof(optionVal));

    //struktura sockAddr
    memset(&sockAddr, '\0', sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1"); nebo INADDR_ANY;
    sockAddr.sin_port = htons(serverPort);

    string serverAddress = inet_ntoa(sockAddr.sin_addr);
    string serverPort = to_string(ntohs(sockAddr.sin_port));
    consoleOut("Server address: " + serverAddress);
    consoleOut("Server port: " + serverPort);

    //Bind socketu
    if (bind(sockfd, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) < 0) {
        consoleOut("Bindování socketu se nezdařilo");
        exit(1);
    }

    //listen
    if (listen(sockfd, MAX_CONNECTED + CONNECT_QUEUE) < 0) {
        consoleOut("Chyba při naslouchání");
        exit(1);
    }

    consoleOut("Inicializace herních místností");
    gameRooms = std::vector<gameRoom *>(MAX_SMALL_ROOMS);

    for (int j = 0; j < MAX_SMALL_ROOMS; ++j) {
        gameRooms.at(j) = new gameRoom();
    }

    for (int i = 0; i < gameRooms.size(); ++i) {
        gameRooms.at(i)->room.numPlaying = 0;
        gameRooms.at(i)->room.maxPlaying = 2;
        gameRooms.at(i)->room.roomName = "Herní místnost " + to_string(i);
        gameRooms.at(i)->room.isFull = false;
        gameRooms.at(i)->room.player = std::vector<players::User>(gameRooms.at(i)->room.maxPlaying);
        gameRooms.at(i)->room.roomId = i;
    }

    sockaddr_in clientSocketAddr;
    int clientSocketAddrSize = sizeof(clientSocketAddr);
    int clientSocket;

    consoleOut("Server spuštěn, čeká na příchozí připojení");
    while (true) {
        FD_ZERO(&socketSet);

        FD_SET(sockfd, &socketSet); //přidání server socketu do setu (Selector)
        max_socketDesc = sockfd;

        for (int i = 0; i < (MAX_CONNECTED); i++) {
            sd = clientSockets[i];
            if (sd > 0) {
                FD_SET(sd, &socketSet);
            }

            if (sd > max_socketDesc) {
                max_socketDesc = sd;
            }
        }

        activity = select(max_socketDesc + 1, &socketSet, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &socketSet)) {
            if ((clientSocket = accept(sockfd, (struct sockaddr *) &clientSocketAddr,
                                       (socklen_t *) &clientSocketAddrSize)) < 0) {
                consoleOut("Chyba při acceptu");
                close(sockfd);
                exit(1);
            }
            for (int i = 0; i < MAX_CONNECTED; i++) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = clientSocket;
                    //cout << "Přidávám nový socket " << clientSocket << " do setu" << endl;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CONNECTED; i++) {
            sd = clientSockets[i];
            if (FD_ISSET(sd, &socketSet)) {
                string incMsg = receiveMsg(sd);
                vector<string> splittedMsg = stl::splitMsg(incMsg);
                switch (msgtable::getType(splittedMsg[0])) {
                    case msgtable::C_LOGIN:
                        if (!loginUsr(sd, splittedMsg[1])) {
                            clientSockets[i] = 0;
                        }
                        break;
                    case msgtable::C_LOGOUT:
                        consoleOut("Hráč s id " + to_string(sd) + " se odpojil");
                        logoutUsr(sd);
                        clientSockets[i] = 0;
                        break;
                    case msgtable::C_GET_TABLE:
                        sendAllRooms(sd);
                        break;
                    case msgtable::C_JOIN_ROOM:
                        assignUsrToRoom(stoi(splittedMsg[1]), sd);
                        break;
                    case msgtable::C_LEAVE_ROOM:
                        removeUsrFromRoom(stoi(splittedMsg[1]), sd);
                        break;
                    case msgtable::C_USR_READY:
                        setUsrReady(stoi(splittedMsg[1]), sd);
                        break;
                    case msgtable::C_USR_NREADY:
                        unsetUsrReady(stoi(splittedMsg[1]), sd);
                        break;
                    case msgtable::C_CHAT:
                        sendUsrMsg(sd, stoi(splittedMsg[1]), splittedMsg[2]);
                        break;
                    case msgtable::C_ROOM_USERS:
                        sendRoomUsers(sd, stoi(splittedMsg[1]));
                        break;
                    case msgtable::C_TURN_CARD:
                        gameRooms.at(stoi(splittedMsg[1]))->turnCard(sd, stoi(splittedMsg[2]), stoi(splittedMsg[3]));
                        break;
                    case msgtable::C_TURN_ACK:
                        gameRooms.at(stoi(splittedMsg[1]))->addTurned();
                    default:
                        break;
                }
                //}
            }
        }

    }
}

void server::consoleOut(string msg) {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "[%d-%m-%Y %H:%M:%S] ", timeinfo);
    std::string str(buffer);

    std::cout << str << msg << endl;
}

void server::sendMsg(int socket, string msg) {
    const char *msgChar = msg.c_str();
    send(socket, (void *) msgChar, msg.length(), 0);
}

string server::receiveMsg(int socket) {
    char msg[128];
    memset(msg, '\0', 128);
    if ((int) read(socket, &msg, 127) < 0) {
        consoleOut("Chyba při příjmání zprávy.");
    }

    int i = 0;
    string msgRet = "";
    while ((msg[i] != '#')) {
        msgRet += msg[i];
        i++;
    }
    return msgRet;
}

bool server::loginUsr(int socket, string name) {
    if (!serverFull) {
        if (nameAvailable(name)) {
            players::User player;
            player.uId = socket;
            player.name = name;
            player.score = 0;
            player.roomId = -1;
            player.isReady = false;

            users.at(connectedUsers) = player;
            connectedUsers++;
            if (connectedUsers >= MAX_CONNECTED) {
                serverFull = true;
            }
            FD_SET(socket, &socketSet);
            sendMsg(socket, ("S_LOGGED:" + name + "#" += '\n'));
            consoleOut("Přihlášen nový hráč " + name + " s id " + to_string(socket));
            return true;
        } else {
            sendMsg(socket, "S_NAME_EXISTS:" + name + "#" += '\n');
            FD_CLR(socket, &socketSet);
            close(socket);
            return false;
        }
    } else {
        sendMsg(socket, "S_SERVER_FULL#" + '\n'); //TODO: nefunguje tak jak má
        FD_CLR(socket, &socketSet);
        close(socket);
        return false;
    }
}

void server::sendAllRooms(int socket) {
    for (int i = 0; i < gameRooms.size(); i++) {
        sendRoomInfo(socket, i);
        string incMsg = receiveMsg(socket);
        vector<string> splittedMsg = stl::splitMsg(incMsg);
        if (splittedMsg[0] == "C_ROW_UPDATE") continue;
        else break;
    }
}

void server::sendRoomInfo(int socket, int roomId) {
    string msg = "S_ROOM_INFO:" +
                 to_string(gameRooms.at(roomId)->room.roomId) + ":" +
                 gameRooms.at(roomId)->room.roomName + ":" +
                 to_string(gameRooms.at(roomId)->room.numPlaying) + ":" +
                 to_string(gameRooms.at(roomId)->room.maxPlaying) + ":" +
                 gameRoom::getString(gameRooms.at(roomId)->roomStatus) +
                 "#" += '\n';
    sendMsg(socket, msg);
}

void server::sendRoomUsers(int socket, int roomId) {
    for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
        sendRoomUserInfo(socket, roomId, i);
        string incMsg = receiveMsg(socket);
        vector<string> splittedMsg = stl::splitMsg(incMsg);
        if (splittedMsg[0] == "C_USER_UPDATE") continue;
        else break;
    }
}

void server::sendRoomUserInfo(int socket, int roomId, int user) {
    string ready = "";
    if(gameRooms.at(roomId)->room.player.at(user).isReady){
        ready = "1";
    }
    else {
        ready = "0";
    }
    string msg =
            "S_ROOM_USER_INFO:" + to_string(user) + ":" + gameRooms.at(roomId)->room.player.at(user).name + ":" + ready + "#" += '\n';
    sendMsg(socket, msg);
}

void server::assignUsrToRoom(int roomId, int playerId) {
    players::User player;
    player = players::getUserById(playerId, users);
    int newRoomId = gameRooms.at(roomId)->addPlayer(player);
    if (newRoomId > -1) {
        users.at(players::getIndexById(playerId, users)).roomId = newRoomId;
        consoleOut("[Místnost " + to_string(roomId) + "] Hráč s id " + to_string(playerId) + " vstoupil do místnosti");
        sendMsg(playerId, "S_USR_JOINED:" + to_string(roomId) + ":" +
                          to_string(gameRooms.at(roomId)->room.numPlaying) + ":" +
                          to_string(gameRooms.at(roomId)->room.maxPlaying) + ":" +
                          gameRoom::getString(gameRooms.at(roomId)->roomStatus) +
                          "#" += '\n');

        int index = 0;
        for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
            if (gameRooms.at(roomId)->room.player.at(i).uId == playerId) {
                index = i;
                break;
            }
        }

        for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
            if (gameRooms.at(roomId)->room.player.at(i).uId != playerId) {
                sendMsg(gameRooms.at(roomId)->room.player.at(i).uId, "S_ROOM_UPDATE:" +
                                                                     to_string(gameRooms.at(roomId)->room.numPlaying) +
                                                                     ":" +
                                                                     gameRoom::getString(
                                                                             gameRooms.at(roomId)->roomStatus) + ":1:" +
                                                                     //1 - připojuje se do místnosti
                                                                     to_string(index) + ":" +
                                                                     gameRooms.at(roomId)->room.player.at(index).name +
                                                                     "#" += '\n');
            }
        }
    } else {
        sendMsg(playerId, "S_JOIN_ERR:" + to_string(roomId) + "#" += '\n');
    }
}

void server::setUsrReady(int roomId, int playerId) {
    if (gameRooms.at(roomId)->setPlayerReady(playerId, true)) {
        users.at(players::getIndexById(playerId, users)).isReady = true;
        if (gameRooms.at(roomId)->allPlayersReady()) {
            for (int i = 0; i < gameRooms.at(roomId)->room.maxPlaying; i++) {
                if(gameRooms.at(roomId)->room.player.at(i).uId != 0) {
                    sendMsg(gameRooms.at(roomId)->room.player.at(i).uId,
                            "S_ROOM_READY:" + to_string(roomId) + ":" + to_string(i) + "#" += '\n');
                }
            }
        } else {
            int index = 0;
            for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
                if (gameRooms.at(roomId)->room.player.at(i).uId == playerId) {
                    index = i;
                    break;
                }
            }

            for (int i = 0; i < gameRooms.at(roomId)->room.maxPlaying; i++) {
                if (gameRooms.at(roomId)->room.player.at(i).uId == playerId) {
                    sendMsg(gameRooms.at(roomId)->room.player.at(i).uId,
                            "S_USR_READY:1:" + to_string(index) + "#" += '\n');
                } else {
                    sendMsg(gameRooms.at(roomId)->room.player.at(i).uId,
                            "S_USR_READY:0:" + to_string(index) + "#" += '\n');
                }
            }
        }
    }
}

void server::unsetUsrReady(int roomId, int playerId) {
    if (gameRooms.at(roomId)->setPlayerReady(playerId, false)) {
        users.at(players::getIndexById(playerId, users)).isReady = false;
        int index = 0;
        for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
            if (gameRooms.at(roomId)->room.player.at(i).uId == playerId) {
                index = i;
                break;
            }
        }
        for (int i = 0; i < gameRooms.at(roomId)->room.maxPlaying; i++) {
            if (gameRooms.at(roomId)->room.player.at(i).uId == playerId) {
                sendMsg(gameRooms.at(roomId)->room.player.at(i).uId,
                        "S_USR_NREADY:1:" + to_string(index) + "#" += '\n');
            } else {
                sendMsg(gameRooms.at(roomId)->room.player.at(i).uId,
                        "S_USR_NREADY:0:" + to_string(index) + "#" += '\n');
            }
        }
    }
}

void server::removeUsrFromRoom(int roomId, int playerId) {
    players::User player;
    player = players::getUserById(playerId, users);
    int index = 0;
    string name = "";
    for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
        if (gameRooms.at(roomId)->room.player.at(i).uId == playerId) {
            index = i;
            name = gameRooms.at(roomId)->room.player.at(i).name;
            break;
        }
    }
    if (gameRooms.at(roomId)->removePlayer(player)) {
        users.at(players::getIndexById(playerId, users)).roomId = -1;
        sendMsg(playerId, "S_USR_LEFT:" + to_string(roomId) + "#" += '\n');

        for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
            sendMsg(gameRooms.at(roomId)->room.player.at(i).uId, "S_ROOM_UPDATE:" +
                                                                 to_string(gameRooms.at(roomId)->room.numPlaying) +
                                                                 ":" +
                                                                 gameRoom::getString(gameRooms.at(roomId)->roomStatus) +
                                                                 ":0:" +
                                                                 to_string(index) + ":" +
                                                                 name +
                                                                 "#" += '\n');
        }
    } else {
        //sendMsg(playerId, "S_JOIN_ERR:" + to_string(roomId) + "#" += '\n');
    }
}

bool server::nameAvailable(string name) {
    for (int i = 0; i < users.size(); i++) {
        if (!name.compare(users.at(i).name))
            return false;
    }
    return true;
}

void server::sendUsrMsg(int playerId, int roomId, string msg) {
    string sender = users.at(players::getIndexById(playerId, users)).name;
    for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
        if (gameRooms.at(roomId)->roomStatus == gameRoom::ROOM_PLAYING)
            sendMsg(gameRooms.at(roomId)->room.player.at(i).uId, "S_CHAT_USR:" + sender + ":" + msg + ":1#" += '\n');
        else
            sendMsg(gameRooms.at(roomId)->room.player.at(i).uId, "S_CHAT_USR:" + sender + ":" + msg + ":0#" += '\n');
    }
}

void server::sendSrvrMsg(int roomId, string msg) {
    for (int i = 0; i < gameRooms.at(roomId)->room.numPlaying; i++) {
        if (gameRooms.at(roomId)->roomStatus == gameRoom::ROOM_PLAYING)
            sendMsg(gameRooms.at(roomId)->room.player.at(i).uId, "S_CHAT_SRVR:" + msg + ":1#" += '\n');
        else
            sendMsg(gameRooms.at(roomId)->room.player.at(i).uId, "S_CHAT_SRVR:" + msg + ":0#" += '\n');
    }
}

void server::logoutUsr(int socket) {
    for (int i = 0; i < users.size(); i++) {
        if ((users.at(i).uId) == socket) {
            if (users.at(i).roomId != -1) {
                gameRooms.at(users.at(i).roomId)->removePlayer(users.at(i));
                users.at(i).roomId = -1;
            }
            users.at(i).uId = 0;
            users.at(i).name = "";
            users.at(i).score = 0;
            users.at(i).isReady = false;
            connectedUsers--;
            serverFull = false;
            FD_CLR(socket, &socketSet);
            close(socket);
            break;
        }
    }
}
