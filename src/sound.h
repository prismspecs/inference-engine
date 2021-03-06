#ifndef _SOUND
#define _SOUND
#include "ofMain.h"

class Sound
{

public:
    // menu
    ofSoundPlayer menu_music;
    ofSoundPlayer menu_select, menu_change;

    // playing
    ofSoundPlayer game_music;
    vector<ofSoundPlayer> engine;
    // vector<ofSoundPlayer> proximity;
    ofSoundPlayer proximity;
    // vector<ofSoundPlayer> proximity_array;
    vector<ofSoundPlayer> vec_change;

    // endgame
    ofSoundPlayer victory;

    // keep track of previous controls so we can look at the difference
    // between that and the incoming controls
    vector<float> prev_controls;

    int last_engine = 0;    // so it doesnt play same engine sound twice

    float pan = 0.0;
    float pan_speed = .0001;

    int proximity_level = 0;
    int prev_proximity_level = 0;

    // music
    bool game_music_fading_in = false;
    bool game_music_fading_out = false;
    long game_music_started_fade = 0;
    int game_music_fade_duration = 3000;
    bool menu_music_fading_in = false;
    bool menu_music_fading_out = false;
    long menu_music_started_fade = 0;
    int menu_music_fade_duration = 1500;

    Sound();
    void setup();
    void update();
    void draw();
    void game_start();
    void game_end();
    void menu_start();
    void menu_end();
    void play_once(string which_sound);
    void play_engine(int active_vec, vector<float> &controls);
    void set_proximity(float dist, float max_dist);

    void change_active_vec(int &v);
};
#endif