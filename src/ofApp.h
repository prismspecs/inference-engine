#pragma once

#include "ofMain.h"
#include "ofxRunway.h"
#include "ofxGui.h"
#include "ofxMidi.h"
#include "imgwarp.h"
#include "surface.h"
#include "controller.h"
#include "ofxGLFWJoystick.h"

// make sure you make your ofApp or class to inherit from ofxRunwayListener
class ofApp : public ofBaseApp, public ofxRunwayListener, public ofxMidiListener
{

public:
	void setup();
	void update();
	void draw();
	void newPosition();
	void exit();

	void keyReleased(int key);
	void keyPressed(int key);

	void drawCenteredText(string str, ofColor color);

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
	void update_position();
	void change_heading(int &av);
	void move_ship(float &direction);
	void receive_controls(vector<float> &controls);
	void lerp_ship();

	// callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson &info);
	void runwayErrorEvent(string &message);

	// gui
	bool USE_GUI = false;
	ofxPanel gui;
	ofParameterGroup sliderGroup;
	vector<ofParameter<float>> vecs;

	// game menu
	ofTrueTypeFont font_menu;
	int menu_selection = 0;
	int menu_count = 0; // how many menu items

	// effects etc
	bool controls_changed = true;
	long last_warp;
	int warp_freq = 200; // how often we should request new image, was 200
	float lerp_amount = 0;
	float lerp_speed = .005;

	// vector<ofParameter<float>> floatSlider;
	vector<int> next_image_loc; // where will next image go? (left or right)
	long last_image_gen;
	int image_gen_freq = 50; // how often we should request new image
#define CURRENT_IMAGE 0
#define TARGET_IMAGE 1

	// game variables
	int img_dims = 1024;
	float truncation = 1;
	float min_max_vecs = 1;
	vector<float> starting_position;
	vector<float> current_position;
	vector<float> target_position;
	vector<int> isolate_vectors;
	int num_isolated = 512;
	bool RANDOMIZE_VECS = true;
#define MENU 0
#define PLAYING 1
#define END 2
	int GAME_STATE = 1;

	// ship vars
	int active_vec = 0;
	// controller
	Controller controller;
	int num_vec_groups = 16; // 32 per group
	int vecs_per_group = 512 / num_vec_groups;
	vector<int> shuffled_vecs;

	// HUD variables
	// fixed target image location and size for minimized target image
	float tiX_min = img_dims - 220;
	float tiY_min = tiX_min;
	float tiD_min = 200;
	// fixed for maximized
	float tiX_max, tiY_max = 0;
	float tiD_max = img_dims;

	bool targetimg_maximized = false; // keep track of whether its minimzed or maximized
	// "current" values so that I can lerp it in and out of minimized/maximized
	float targetimg_x = tiX_min;
	float targetimg_y = tiY_min;
	float targetimg_dim = tiD_min;

	float distance = 0.0; // how far from destination

	// MIDI
	void newMidiMessage(ofxMidiMessage &eventArgs);
	ofxMidiIn midiIn;
	std::vector<ofxMidiMessage> midiMessages;
	std::size_t maxMessages = 10; // max number of messages to keep track of

	// debug, etc
	int counter = 0;
	bool save_frames = false;
};
