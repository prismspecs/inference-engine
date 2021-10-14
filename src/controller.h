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
    void reset(vector<float> current_pos, vector<float> target_pos, vector<int> shuffled_vecs);
    double getAngleRads(ofVec2f a, ofVec2f b);
    void keyPressed(ofKeyEventArgs &e);
    void keyReleased(ofKeyEventArgs &e);
    void receive_serial(int &inByte);

    vector<float> controls;
    vector<float> target_controls;
    int num_control_vecs = 16;
    int num_controls_root;
    int active_vec = 0;
    float vector_speed = .01;

    // drawing
    float controls_start_x = 1920/2 + 512 + 30;
    float controls_start_y = 56 + 50;
    float controls_dims = 408;

    // diagnostic
    bool USE_GAMEPAD = false;
    bool USE_KEYBOARD = true;
    
    int num_vecs = 0;
};
#endif