#ifndef _CONTROLLER
#define _CONTROLLER
#include "ofMain.h"

class Controller
{

public:
    ofEvent<vector<float>> sendControlVectors;
    ofEvent<string> buttonPress;  // menu controls...


    Controller();
    void setup(int ncv, float vectorspeed);
    void update();
    void draw();
    void reset(vector<float> z, vector<int> shuffled_vecs);
    double getAngleRads(ofVec2f a, ofVec2f b);
    void keyPressed(ofKeyEventArgs &e);
    void keyReleased(ofKeyEventArgs &e);

    vector<float> controls;
    int num_control_vecs = 16;
    int active_vec = 0;
    float vector_speed = .01;

    // diagnostic
    bool USE_GAMEPAD = false;
    bool USE_KEYBOARD = true;
    
    int num_vecs = 0;
};
#endif