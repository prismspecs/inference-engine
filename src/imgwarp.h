#ifndef _IMGWARP
#define _IMGWARP
#include "ofMain.h"

class ImageWarp
{

public:
    ImageWarp(ofTexture & _tex, ofVec3f _pos);
    void update();
    void draw();

    // variables
    ofImage img;

    ofVec3f pos;
    bool fly = false;
    ofTexture tex;
    ofMesh mesh;
    float speed = 8; // was 10

    float alpha = 70;  // was 100
    float alphaDrain = 6;   // was 4
    float scale = 512;
};
#endif