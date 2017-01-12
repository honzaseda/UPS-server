#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include "server.h"


using namespace std;

int main(int argc, char *argv[]) {
    server *newServer = new server();
    if(argc == 2){
        newServer->setServerPort(atoi(argv[1]));
    }
    newServer->start();
}