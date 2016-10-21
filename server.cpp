//
// Created by Seda on 20. 10. 2016.
//

#include <unistd.h>
#include <stdlib.h>
#include "server.h"

using namespace std;

server::server(int port) {
    this->serverPort = port;
}

void server::start() {
    //vytvoření socketu
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0) {
        cout << "Chyba při vytvoření socketu";
        exit(1);
    }
    cout << "Server běží na portu" << serverPort;

    int optionVal = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optionVal, sizeof(optionVal));

    //struktura sockAddr
    memset(&sockAddr, '\0', sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    sockAddr.sin_port = htons( serverPort );

    //Bind socketu
    if( bind(sockfd,(struct sockaddr *) &sockAddr , sizeof(sockAddr)) < 0)
    {
        cout << "Bindování socketu se nezdařilo";
        exit(1);
    }

    //listen
    if( listen(sockfd , maxConnected) < 0 );
    {
        cout << "Chyba při naslouchání";
        exit(1);
    }

}


