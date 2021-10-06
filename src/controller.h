#ifndef _CONTROLLER
#define _CONTROLLER
#include "ofMain.h"

class Controller
{

public:
    Controller();
    void setup();
    void update();
    void draw();
    double getAngleRads(ofVec2f a, ofVec2f b);

    int active_vec = 0;

};
#endif