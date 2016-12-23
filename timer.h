//
// Created by seda on 22/12/16.
//

#ifndef UPS_SERVER_TIMER_H
#define UPS_SERVER_TIMER_H

#include <time.h>

class timer {
private:
    unsigned long begTime;
public:
    void start();

    unsigned long elapsedTime();

    bool isTimeout(unsigned long seconds);
};

#endif //UPS_SERVER_TIMER_H
