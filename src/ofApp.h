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
	
	ofImage runwayResult;
		
	vector<float> generate_random_z();
	void generate_image(vector<float> z, float truncation);
	
	// Callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson& info);
	void runwayErrorEvent(string& message);
	
	bool bWaitingForResponse = false;

	// gui
	ofxPanel gui;
	ofParameterGroup sliderGroup;
	ofParameter<float> vec0;
	ofParameter<float> vec1;
	ofParameter<float> vec2;
	ofParameter<float> vec3;
	ofParameter<float> vec4;
	ofParameter<float> vec5;
	ofParameter<float> vec6;
	ofParameter<float> vec7;
	ofParameter<float> vec8;
	ofParameter<float> vec9;
	// ofParameter<float> vec10;
	// ofParameter<float> vec11;
	// ofParameter<float> vec12;

	// vector<ofParameter<float>> floatSlider;


	// game variables
	float truncation = 1.3;
	vector<float> starting_position;
	vector<float> target_position;
	vector<int> isolate_vectors;
	int num_isolated = 10;
};
