//
// Created by Seda on 20. 10. 2016.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>

#ifndef UPS_SERVER_SERVER_H
#define UPS_SERVER_SERVER_H

class server {
    int serverPort;
    int sockfd;
    int maxConnected = 4;

    struct sockaddr_in sockAddr;
public:
    server(int port);
    void start();
    void sendMsg(int socket, std::string msg);
    std::string receiveMsg(int socket);
    void loginUsr();
};

#endif //UPS_SERVER_SERVER_H
