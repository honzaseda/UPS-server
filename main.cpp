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

const int SERVER_PORT = 44444;

int main() {
    server* newServer = new server(SERVER_PORT);
    newServer->start();
}