#pragma once

#include "ofMain.h"
#include "ofxRunway.h"
#include "ofxGui.h"

// make sure you make your ofApp or class to inherit from ofxRunwayListener
class ofApp : public ofBaseApp, public ofxRunwayListener
{

public:
	void setup();
	void update();
	void draw();
	void newPosition();

	void keyReleased(int key);
	void keyPressed(int key);

	// effects
	ofMesh make_mesh(ofImage image);
	void draw_stars(ofImage image);

	ofxRunway runway;

	ofImage currentImg;
	ofImage targetImg;

	vector<float> generate_random_z();
	void generate_image(vector<float> z, float truncation, int next_loc);
	vector<float> generate_new_target(vector<float> startPos, vector<int> iso_vecs);
	float find_distance(vector<float> a, vector<float> b);
	void control_changed(ofAbstractParameter &e);

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
	int num_isolated = 18;

	float distance = 0.0; // how far from destination
};
