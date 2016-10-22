//
// Created by seda on 22/10/16.
//
#include <vector>
#include <string>

#ifndef UPS_SERVER_STL_H
#define UPS_SERVER_STL_H

#define MSG_DELIMITER ':'

class stl{
public:
    static std::vector<std::string> splitMsg(std::string msg);
};

#endif //UPS_SERVER_STL_H
