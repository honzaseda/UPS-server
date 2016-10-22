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
    if(listen(sockfd, maxConnected) < 0 ){
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
        for (size_t i = 0; i < splittedMsg.size(); i++) {
            cout << "\"" << splittedMsg[i] << "\"" << endl;
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

void server::loginUsr(){

}

