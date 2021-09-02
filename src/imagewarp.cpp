#include "imgwarp.h"

ImageWarp::ImageWarp(ofTexture & _tex, ofVec3f _pos)
{

    pos = _pos;
    tex = _tex;

    // create a quad mesh
    // first triangle
    mesh.addVertex(ofVec3f(-1, -1, 0));
    mesh.addVertex(ofVec3f(1, -1, 0));
    mesh.addVertex(ofVec3f(1, 1, 0));

    // second triangle
    mesh.addVertex(ofVec3f(1, 1, 0));
    mesh.addVertex(ofVec3f(-1, 1, 0));
    mesh.addVertex(ofVec3f(-1, -1, 0));

    // first triangle
    mesh.addTexCoord(ofVec2f(0, 0));
    mesh.addTexCoord(ofVec2f(1, 0));
    mesh.addTexCoord(ofVec2f(1, 1));

    // second triangle
    mesh.addTexCoord(ofVec2f(1, 1));
    mesh.addTexCoord(ofVec2f(0, 1));
    mesh.addTexCoord(ofVec2f(0, 0));
}

void ImageWarp::update()
{
    if (fly)
    {
        alpha -= alphaDrain;
        pos.z += speed;
    }
}

void ImageWarp::draw()
{
    ofPushMatrix();
    ofTranslate(pos);
    ofScale(scale, scale, 1);
    ofSetColor(255, 255, 255, alpha);
    tex.bind();
    mesh.draw();
    tex.unbind();
    ofPopMatrix();
}