#ifndef _VECDISPLAY
#define _VECDISPLAY
#include "ofMain.h"

class VecDisplay
{

public:

    VecDisplay();
    void setup(float _x, float _y, float _dim);
    void update();
    void draw();
    double getAngleRads(ofVec2f a, ofVec2f b);

    ofVec2f pos;
    float dim;
    float diff = 0.0; // difference from correct vector
    bool active = false;
    
    // effects
    ofColor color;

    // heartbeat style monitor
    int wave_resolution = 40;
    float phase;
    float phase_speed = .03;
    float max_phase_speed = .08;
    float phase_amplitude = 1;
    float max_phase_amplitude = 20;

};
#endif