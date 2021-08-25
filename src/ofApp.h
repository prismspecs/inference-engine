#pragma once

#include "ofMain.h"
#include "ofxRunway.h"
#include "ofxGui.h"
#include "ofxMidi.h"
#include "imgwarp.h"

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

	// effects
	ofMesh make_mesh(ofImage image);
	void draw_stars(ofImage image);
	// image warp, effect of images zooming towards camera
	vector<ImageWarp> warps;


	ofxRunway runway;

	ofImage currentImg;
	ofImage targetImg;
	ofFbo currentFbo;
	ofFbo targetFbo;

	vector<float> generate_random_z();
	void generate_image(vector<float> z, float truncation, int next_loc);
	vector<float> generate_new_target(vector<float> startPos, vector<int> iso_vecs);
	float find_distance(vector<float> a, vector<float> b);
	void control_changed(ofAbstractParameter &e);
	void update_position();

	// callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson &info);
	void runwayErrorEvent(string &message);

	bool bWaitingForResponse = false;
	bool bWaitingForTarget = false;

	// gui
	ofxPanel gui;
	ofParameterGroup sliderGroup;
	vector<ofParameter<float>> vecs;

	// vector<ofParameter<float>> floatSlider;
	vector<int> next_image_loc; // where will next image go? (left or right)
	long last_image_gen;
	int image_gen_freq = 50;	// how often we should request new image
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
	int num_isolated = 32;

	// HUD variables
	// fixed target image location and size for minimized target image
	float tiX_min = img_dims - 220;
	float tiY_min = tiX_min;
	float tiD_min = 200;
	// fixed for maximized
	float tiX_max, tiY_max = 0;
	float tiD_max = img_dims;

	bool targetimg_maximized = false;	// keep track of whether its minimzed or maximized
	// "current" values so that I can lerp it in and out of minimized/maximized
	float targetimg_x = tiX_min;
	float targetimg_y = tiY_min;
	float targetimg_dim = tiD_min;


	float distance = 0.0; // how far from destination

	// MIDI
	void newMidiMessage(ofxMidiMessage& eventArgs);
	ofxMidiIn midiIn;
	std::vector<ofxMidiMessage> midiMessages;
	std::size_t maxMessages = 10; // max number of messages to keep track of

	// debug
	int counter = 0;
};
