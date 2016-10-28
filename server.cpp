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

server::server(int port) {
    this->users.resize((unsigned long) (MAX_CONNECTED));
    this->serverPort = port;
}

void server::start() {
    connectedUsers = 0;
    serverFull = false;
    sockfd = -1;

    for (int i = 0; i < MAX_CONNECTED; i++)
    {
        clientSockets[i] = 0;
    }
    //vytvoření socketu
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0) {
        cout << "Chyba při vytvoření socketu" << endl;
        exit(1);
    }
    cout << "Server port: " << serverPort << endl;

    int optionVal = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optionVal, sizeof(optionVal));

    //struktura sockAddr
    memset(&sockAddr, '\0', sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1"); nebo INADDR_ANY;
    sockAddr.sin_port = htons( serverPort );

    //Bind socketu
    if( bind(sockfd,(struct sockaddr *) &sockAddr , sizeof(sockAddr)) < 0)
    {
        cout << "Bindování socketu se nezdařilo" << endl;
        exit(1);
    }

    //listen
    if(listen(sockfd, MAX_CONNECTED + CONNECT_QUEUE) < 0 ){
        cout << "Chyba při naslouchání" << endl;
        exit(1);
    }
    cout << "Server spuštěn, čeká na příchozí připojení" << endl;

    this->gameRooms = std::vector<gameRoom*>(MAX_SMALL_ROOMS);

    for (int j = 0; j < MAX_SMALL_ROOMS; ++j) {
        this->gameRooms.at(j) = new gameRoom();
    }

    for (int i = 0; i < this->gameRooms.size(); ++i) {
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
    while(true){
        FD_ZERO(&socketSet);

        FD_SET(sockfd, &socketSet); //přidání server socketu do setu (Selector)
        max_socketDesc = sockfd;

        for (int i = 0; i < (MAX_CONNECTED); i++){
            sd = clientSockets[i];
            if(sd > 0){
                FD_SET(sd, &socketSet);
            }

            if(sd > max_socketDesc){
                max_socketDesc = sd;
            }
        }

        activity = select(max_socketDesc + 1, &socketSet, NULL, NULL, NULL);

        if(FD_ISSET(sockfd, &socketSet)) {
            if ((clientSocket = accept(sockfd, (struct sockaddr *) &clientSocketAddr,(socklen_t *) &clientSocketAddrSize)) < 0) {
                cout << "Chyba při acceptu";
                close(sockfd);
                exit(1);
            }
            for (int i = 0; i < MAX_CONNECTED; i++){
                if(clientSockets[i] == 0){
                    clientSockets[i] = clientSocket;
                    cout << "Přidávám nový socket " << clientSocket << " do setu" << endl;
                    break;
                }
            }
        }

        for(int i = 0; i < MAX_CONNECTED; i++) {
            sd = clientSockets[i];
            if(FD_ISSET(sd, &socketSet)) {
                    string incMsg = receiveMsg(sd);
                    vector<string> splittedMsg = stl::splitMsg(incMsg);
                    switch (msgtable::getType(splittedMsg[0])) {
                        case msgtable::C_LOGIN:
                            if (!loginUsr(sd, splittedMsg[1])) {
                                clientSockets[i] = 0;
                            }
                            break;
                        case msgtable::C_LOGOUT:
                            cout << "Hráč s id " << sd << " se odpojil" << endl;
                            logoutUsr(sd);
                            clientSockets[i] = 0;
                            break;
                        case msgtable::C_GET_TABLE:
                            sendAllRooms(sd);
                            break;
                        case msgtable::C_JOIN_ROOM:
                            assignUsrToRoom(stoi(splittedMsg[1]));
                            break;
                        default:
                            break;
                    }
                //}
            }
        }

    }
}

void server::sendMsg(int socket, string msg){
    const char* msgChar = msg.c_str();
    send(socket, (void *)msgChar, msg.length(), 0);
}

string server::receiveMsg(int socket){
    char msg[128];
    memset(msg, '\0', 128);
    if ((int) read(socket, &msg, 127) < 0) {
        cout << "Chyba při příjmání zprávy.";
    }

    int i = 0;
    string msgRet = "";
    while ((msg[i] != '#')) {
        msgRet += msg[i];
        i++;
    }
    return msgRet;
}

bool server::loginUsr(int socket, string name){
    if(!serverFull) {
        if(nameAvailable(name)) {
            players::User player;
            player.uId = socket;
            player.name = name;
            player.score = 0;
            player.isReady = false;

            this->users.at(connectedUsers) = player;
            connectedUsers++;
            if (connectedUsers >= MAX_CONNECTED) {
                serverFull = true;
            }
            FD_SET(socket, &socketSet);
            sendMsg(socket, ("S_LOGGED:" + name + "#" += '\n'));
            cout << "Přihlášen nový hráč " << name << " s id " << socket << endl;
            return true;
        }
        else {
            sendMsg(socket, "S_NAME_EXISTS:" + name + "#" += '\n');
            FD_CLR(socket, &socketSet);
            close(socket);
            return false;
        }
    }
    else {
        sendMsg(socket, "S_SERVER_FULL#" + '\n'); //TODO: nefunguje tak jak má
        FD_CLR(socket, &socketSet);
        close(socket);
        return false;
    }
}

void server::sendAllRooms(int socket){
    for(int i=0; i<gameRooms.size(); i++){
        sendRoomInfo(socket, i);
    }
}

void server::sendRoomInfo(int socket, int roomId){
    //nanosleep((const struct timespec[]){{0, 5000000L}}, NULL);
    string msg = "S_ROOM_INFO:" +
                 to_string(gameRooms.at(roomId)->room.roomId) + ":" +
                 gameRooms.at(roomId)->room.roomName + ":" +
                 to_string(gameRooms.at(roomId)->room.numPlaying) + ":" +
                 to_string(gameRooms.at(roomId)->room.maxPlaying) + ":" +
                 to_string(gameRooms.at(roomId)->roomStatus) +
                 "#" += '\n';
    sendMsg(socket, msg);
}

void server::assignUsrToRoom(int roomId){

}

bool server::nameAvailable(string name){
    for(int i=0; i<users.size(); i++){
        if(!name.compare(users.at(i).name))
            return false;
    }
    return true;
}

void server::logoutUsr(int socket){
    for(int i=0; i<users.size(); i++){
        if((users.at(i).uId) == socket){
            users.at(i).uId = 0;
            users.at(i).name = "";
            connectedUsers--;
            serverFull = false;
            FD_CLR(socket, &socketSet);
            close(socket);
            break;
        }
    }
}
