//
// Created by Seda on 20. 10. 2016.
//

#include <unistd.h>
#include <stdlib.h>
#include "server.h"
#include "stl.h"
#include "msgtable.h"

using namespace std;

server::server(int port) {
    this->serverPort = port;
}

void server::start() {
    connectedUsers = 0;
    serverFull = false;
    sockfd = -1;

    //vytvoření socketu
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0) {
        cout << "Chyba při vytvoření socketu" << endl;
        exit(1);
    }
    cout << "Server běží na portu " << serverPort << endl;

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
    if(listen(sockfd, MAX_CONNECTED) < 0 ){
        cout << "Chyba při naslouchání" << endl;
        exit(1);
    }
    cout << "Server spuštěn, čeká na příchozí připojení" << endl;

    sockaddr_in clientSocketAddr;
    int clientSocketAddrSize = sizeof(clientSocketAddr);
    int clientSocket;
    while(true){
        if((clientSocket = accept(sockfd, (struct sockaddr *)&clientSocketAddr, (socklen_t*)&clientSocketAddrSize)) < 0) {
            cout << "Chyba při acceptu";
            close(sockfd);
            exit(1);
        }
        string incMsg = receiveMsg(clientSocket);
        vector<string> splittedMsg = stl::splitMsg(incMsg);
        //if(splittedMsg.size() > 1){
            switch (msgtable::getType(splittedMsg[0])) {
                case msgtable::C_LOGIN:
                    loginUsr(clientSocket, splittedMsg[1]);
                    break;
                case msgtable::C_LOGOUT:
                    //logoutUsr();
                    break;
                case msgtable::C_GET_TABLE:
                    //sendTable();
                    break;
                default:
                    break;
            }
        //}
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

void server::loginUsr(int socket, string name){
    if(!serverFull) {
        if(nameAvailable(name)) {
            this->users[connectedUsers].uId = socket;
            this->users[connectedUsers].name = name;
            connectedUsers++;
            if (connectedUsers >= MAX_CONNECTED) {
                serverFull = true;
            }
            sendMsg(socket, ("Byl jsi úspěšně přihlášen na jméno " + name));
            cout << "Přihlášen nový hráč " << name << " s id " << socket << endl;
        }
        else sendMsg(socket, "Přihlášení se nezdařilo, uživatel se jménem " + name + " již existuje");

    }
    else sendMsg(socket, "Přihlášení se nezdařilo, server je plný");
}

bool server::nameAvailable(string name){
    for(int i=0; i<connectedUsers; i++){
        if(!name.compare(this->users[i].name))
            return false;
    }
    return true;
}
