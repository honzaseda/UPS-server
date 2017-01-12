//
// Created by seda on 22/10/16.
//

#include "stl.h"

using namespace std;

/**
 * Rozdělí příchozí zprávu podle rozdělujícího znaku
 * @param msg Příchozí zpráva
 * @return Vektor
 */
vector<string> stl::splitMsg(string msg) {
    string next;
    vector<string> result;

    for (string::const_iterator it = msg.begin(); it != msg.end(); it++) {
        if (*it != MSG_DELIMITER) {
            next += *it;
        } else {
            if (!next.empty()) {
                result.push_back(next);
                next.clear();
            }
        }
    }
    if (!next.empty())
        result.push_back(next);
    return result;
}