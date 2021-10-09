#ifndef _CONTROLLER
#define _CONTROLLER
#include "ofMain.h"

class Controller
{

public:
    ofEvent<int> headingChange;
    ofEvent<float> moveShip;
    ofEvent<vector<float>> sendControls;

    Controller();
    void setup(int ncv);
    void update();
    void draw();
    double getAngleRads(ofVec2f a, ofVec2f b);
    void keyPressed(ofKeyEventArgs &e);
    void keyReleased(ofKeyEventArgs &e);

    vector<float> controls;
    int num_control_vecs = 16;
    int active_vec = 0;

    // diagnostic
    bool USE_GAMEPAD = false;
    bool USE_KEYBOARD = true;

    
    int num_vecs = 0;
};
#endif