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
    float speed = 15;

    float alpha = 255;
    float alphaDrain = 12;   // was 4
    float scale = 512;
};
#endif