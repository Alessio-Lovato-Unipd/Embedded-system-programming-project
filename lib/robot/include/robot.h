#ifndef ROBOT_H
#define ROBOT_H

#include "mappa.h"
#include <iostream>

using std::pair;
typedef pair<float, float> posizione; // corrisponderà a (x,y)


class Robot
{
public:
    Robot(const posizione &robot, const posizione &obbiettivo, mappa &mappa_riferimento, const float raggio_robot = 0.5);
private:
    posizione robot_;
    posizione obbiettivo_;
    mappa mappa_;
    float raggio_;

    void incrementa_mappa(const posizione &posizione_necessaria);
    void centra_posizione(float &pos);
};




#endif