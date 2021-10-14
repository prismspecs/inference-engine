#include "surface.h"

Surface::Surface()
{
}

void Surface::setup(ofVec3f pos, int dims)
{
    position = pos;
    dimensions = dims;

    // use floating point fbo to eliminate trails problem
    fbo.allocate(dimensions, dimensions, GL_RGBA32F_ARB);

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

void Surface::update(ofTexture &tex)
{
    texture = tex;
}

void Surface::draw()
{
    if (texture.isAllocated())
    {

        ofEnableAlphaBlending();
        fbo.begin();
        ofSetColor(255, 255, 255, 10);
        texture.draw(0, 0);
        fbo.end();
        ofDisableAlphaBlending();

        //fbo.draw(0, 0);

        ofPushMatrix();
        ofTranslate(position);
        ofScale(dimensions / 2, dimensions / 2, 1);
        ofSetColor(255, 255, 255, 255);
        fbo.getTexture().bind();
        mesh.draw();
        fbo.getTexture().unbind();
        ofPopMatrix();
    }
}