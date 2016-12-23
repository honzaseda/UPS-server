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

int main() {
    server *newServer = new server();
    newServer->start();
}