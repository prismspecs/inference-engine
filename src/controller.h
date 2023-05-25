#ifndef _CONTROLLER
#define _CONTROLLER
#include "ofMain.h"
#include "vector_display.h"

class Controller
{

public:
    ofEvent<vector<float>> sendControlVectors;
    ofEvent<string> buttonPress;  // menu controls...
    ofEvent<int> activeVecChanged;

    Controller();
    void setup(int ncv, float vectorspeed);
    void update();
    void draw();
    void reset(vector<float> current_pos, vector<float> target_pos, vector<int> shuffled_vecs);
    double getAngleRads(ofVec2f a, ofVec2f b);
    void keyPressed(ofKeyEventArgs &e);
    void keyReleased(ofKeyEventArgs &e);
    void receive_serial(int &inByte);
    void lerp_ship();

    // diagnostic lerping
    float lerp_amount = 0;
    float lerp_speed = .0001;

    vector<float> controls;
    vector<float> target_controls;
    int num_control_vecs = 16;
    int num_controls_root;
    int active_vec = 0;
    float vector_speed = .01;  // was .01

    // drawing vector readouts
    vector<VecDisplay> vec_displays;
    float controls_start_x = 20;
    float controls_start_y = 348; // was 162
    float controls_dim_x = 408;
    float controls_dim_y = 408; // was 700
    float control_dim_x = controls_dim_x/4;
    float control_dim_y = controls_dim_y/4;


    // diagnostic
    bool USE_GAMEPAD = false;
    bool USE_KEYBOARD = true;
    
    int num_vecs = 0;
};
#endif