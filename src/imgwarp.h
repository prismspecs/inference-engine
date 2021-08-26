#ifndef _IMGWARP 
#define _IMGWARP 
#include "ofMain.h" 

class ImageWarp {

    public: 

    void setup();  
    void update(); 
    void draw(); 

    // variables
    ofImage img;
    float alpha = 255;
    float alphaDrain = 10;
    float scale = 1;
    float scaleSpeed = .01;

    ImageWarp(ofImage image); 
}; 
#endif