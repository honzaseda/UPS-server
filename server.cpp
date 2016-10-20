//
// Created by Seda on 20. 10. 2016.
//

#include <unistd.h>
#include <stdlib.h>
#include "server.h"

using namespace

void server::start() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
}


