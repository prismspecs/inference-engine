#ifndef _SURFACE
#define _SURFACE
#include "ofMain.h"

class Surface
{

public:
    Surface();
    void setup(ofVec3f pos, int dims);
    void update(ofTexture &tex);
    void draw();

    // variables
    ofImage image;
    ofTexture texture;
    ofMesh mesh;
    ofFbo fbo;
    ofVec3f position;
    int dimensions;
    bool first_run = true;
};
#endif