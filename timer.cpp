//
// Created by seda on 22/12/16.
//

#include "timer.h"

void timer::start() {
    clock_gettime(CLOCK_MONOTONIC, &this->startTime);
}

double timer::elapsedTime() {
    clock_gettime(CLOCK_MONOTONIC, &this->finishTime);
    elapsed = (finishTime.tv_sec - startTime.tv_sec);
    elapsed += (finishTime.tv_nsec - startTime.tv_nsec) / 1000000000.0;
    return elapsed;
}