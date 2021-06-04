#pragma once

#include "ofMain.h"
#include "ofxRunway.h"
#include "ofxGui.h" 

// make sure you make your ofApp or class to inherit from ofxRunwayListener
class ofApp : public ofBaseApp, public ofxRunwayListener{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyReleased(int key);
		
	ofxRunway runway;
	
	ofImage currentImg;
	ofImage targetImg;
		
	vector<float> generate_random_z();
	void generate_image(vector<float> z, float truncation);
	vector<float> generate_new_target(vector<float> startPos, vector<int> iso_vecs);
	float find_distance(vector<float> a, vector<float> b);
	void control_changed(ofAbstractParameter& e);
	
	// Callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson& info);
	void runwayErrorEvent(string& message);
	
	bool bWaitingForResponse = false;
	bool bWaitingForTarget = false;

	// gui
	ofxPanel gui;
	ofParameterGroup sliderGroup;
	vector<ofParameter<float>> vecs;


	// vector<ofParameter<float>> floatSlider;


	// game variables
	int img_dims = 512;
	float truncation = 1.1;
	float min_max_vecs = 1;
	vector<float> starting_position;
	vector<float> current_position;
	vector<float> target_position;
	vector<int> isolate_vectors;
	int num_isolated = 50;
};
