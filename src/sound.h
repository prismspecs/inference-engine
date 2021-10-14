#ifndef _SOUND
#define _SOUND
#include "ofMain.h"

class Sound
{

public:

    ofSoundPlayer music;
    ofSoundPlayer menu_select, menu_change;
    ofSoundPlayer engine;

    // keep track of previous controls so we can look at the difference
    // between that and the incoming controls
    vector<float> prev_controls;

    float pan = 0.0;
    float pan_speed = .0001;

    Sound();
    void setup();
    void update();
    void draw();
    void play_once(string which_sound);
    void play_engine(int active_vec, vector<float> &controls);

};
#endif