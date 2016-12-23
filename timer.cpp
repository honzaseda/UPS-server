//
// Created by seda on 22/12/16.
//

#include "timer.h"

void timer::start() {
    begTime = clock();
}

unsigned long timer::elapsedTime() {
    return ((unsigned long) clock() - begTime) / CLOCKS_PER_SEC;
}

bool timer::isTimeout(unsigned long seconds) {
    return seconds >= elapsedTime();
}