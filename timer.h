//
// Created by seda on 22/12/16.
//

#ifndef UPS_SERVER_TIMER_H
#define UPS_SERVER_TIMER_H

#include <time.h>

class timer {
private:
    struct timespec startTime, finishTime;
    double elapsed;
public:
    void start();

    double elapsedTime();
};

#endif //UPS_SERVER_TIMER_H
