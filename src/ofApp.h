#pragma once

#include "ofMain.h"
#include "ofxRunway.h"
#include "ofxGui.h"
#include "imgwarp.h"
#include "surface.h"
#include "controller.h"
#include "sound.h"
#include "ofxSerial.h"

// make sure you make your ofApp or class to inherit from ofxRunwayListener
class ofApp : public ofBaseApp, public ofxRunwayListener
{

public:
    void setup();
    void update();
    void draw();
    void draw_distance_indicator(float &distance);
    void newPosition();

    void keyReleased(int key);
    void keyPressed(int key);

    void draw_centered_text(string str, ofColor color, float offset);

    // surfaces (main image contexts for game)
    Surface surface_main;

    // effects
    ofMesh make_mesh(ofImage image);
    void draw_stars(ofImage image);
    void warp_effect(ofTexture &texture, ofVec3f location);
    // image warp, effect of images zooming towards camera
    vector<ImageWarp> warps;
    ofVec3f origin; // center of the screen

    ofxRunway runway;

    ofImage currentImg;
    ofImage targetImg;
    ofFbo currentFbo;
    ofFbo targetFbo;

    vector<float> generate_random_z();
    vector<float> generate_random_grouped_z();
    void generate_image(vector<float> z, float truncation, int next_loc);
    vector<float> generate_new_target(vector<float> startPos, vector<int> iso_vecs);
    float find_distance(vector<float> a, vector<float> b);
    void control_changed(ofAbstractParameter &e);
    void receive_button(string &button);
    void receive_control_vectors(vector<float> &controls);
    void change_active_vec(int &vec);
    void lerp_ship();

    // callback functions that process what Runway sends back
    void runwayInfoEvent(ofJson &info);
    void runwayErrorEvent(string &message);

    // serial
    ofSerial serial;
    // ofx::IO::SerialDevice serial;

    // gui
    bool USE_GUI = false;
    ofxPanel gui;
    ofParameterGroup sliderGroup;
    vector<ofParameter<float>> vecs;

    // game menu
    ofTrueTypeFont font_menu;
    int menu_selection = 0;
    int menu_count = 1; // how many menu items
    bool menu_startfade = false;
    int menu_fadetime = 1000;
    long menu_fadestartedtime = 0;

    // PLAYING state stuff
    bool controls_changed = true;
    long last_warp;
    int warp_freq = 40; // how often we should request new image, was 200
    float lerp_amount = 0;
    float lerp_speed = .003;
    bool game_startfade = false;
    int game_fadetime = 1500;
    long game_fadestartedtime = 0;
    long last_time_controls_changed = 0;
    int controls_idle_timeout = 60000; // how long until game resets

    // tutorial stuff
    ofVideoPlayer tutorial_vid;

    // end game stuff
    bool end_startfade = false;
    int end_fadetime = 8000;
    long end_fadestartedtime = 0;

    vector<int> next_image_loc; // where will next image go? (left or right)
    long last_image_gen;
    int image_gen_freq = 50; // how often we should request new image
#define CURRENT_IMAGE 0
#define TARGET_IMAGE 1

    // game variables
    float global_vector_speed = .01;
    int img_dims = 1024;
    float truncation = .9;   // was .9
    float min_max_vecs = 1;
    vector<float> starting_position;
    vector<float> current_position;
    vector<float> target_position;
    vector<int> isolate_vectors;
    // int num_isolated = 512;
    bool RANDOMIZE_VECS = true;
#define MENU 0
#define PLAYING 1
#define END 2
#define HELP_SCREEN 3
    int GAME_STATE = 0;

    // ship vars
    int active_vec = 0;
    // controller
    Controller controller;
    int num_vec_groups = 16; // 32 per group
    int vecs_per_group = 512 / num_vec_groups;
    vector<int> shuffled_vecs;

    // HUD variables
    // fixed target image location and size for minimized target image
    float tiX_min = 1492;
    float tiY_min = 390; // was 584
    float tiD_min = 408;
    // fixed for maximized
    float tiX_max = 448;
    float tiY_max = 28;
    float tiD_max = img_dims;

    bool targetimg_maximized = false; // keep track of whether its minimzed or maximized
    // "current" values so that I can lerp it in and out of minimized/maximized
    float targetimg_x = tiX_min;
    float targetimg_y = tiY_min;
    float targetimg_dim = tiD_min;

    float distance = 0.0; // how far from destination
    float starting_distance = 0.0;
    float max_dist = sqrt(4 * 512);
    float min_distance = 2.5; // minimum distance required to win, was 2.2

    // sound
    Sound sound;

    // debug, etc
    int counter = 0;
    bool save_frames = false;
};
