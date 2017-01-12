//
// Created by Seda on 20. 10. 2016.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include "gameRoom.h"


#ifndef UPS_SERVER_SERVER_H
#define UPS_SERVER_SERVER_H

#define MAX_CONNECTED 32
#define CONNECT_QUEUE 5
#define MAX_SMALL_ROOMS 6

class server {
    int serverPort;
    int sockfd;
    int connectedUsers;
    bool serverFull;
    int max_socketDesc;
    int sd;
    int clientSockets[MAX_CONNECTED];
    int activity;

    fd_set socketSet;

    struct sockaddr_in sockAddr;

    std::vector<gameRoom *> gameRooms;
    std::vector<players::User> users;
public:
    server();

    void start();

    static void consoleOut(std::string msg);

    void sendMsg(int socket, std::string msg);

    std::string receiveMsg(int socket);

    bool loginUsr(int socket, std::string name);

    bool alreadyConnected(int socket);

    bool nameAvailable(std::string name);

    void logoutUsr(int socket);

    void assignUsrToRoom(int roomId, int playerId);

    void setUsrReady(int roomId, int playerId);

    void unsetUsrReady(int roomId, int playerId);

    void removeUsrFromRoom(int roomId, int playerId);

    void sendAllRooms(int socket);

    void sendRoomInfo(int socket, int roomId);

    void sendUsrMsg(int playerId, int roomId, std::string msg);

    void sendTimeMsg(gameRoom *r, int id);

    void sendRoomUsers(int socket, int roomId);

    void sendRoomUserInfo(int socket, int roomId, int user);
};

#endif //UPS_SERVER_SERVER_H
