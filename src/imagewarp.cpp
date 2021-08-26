#include "imgwarp.h"

ImageWarp::ImageWarp(ofImage image)
{
    img = image;
}

void ImageWarp::setup()
{
}

void ImageWarp::update()
{
    alpha -= alphaDrain;
    scale += scaleSpeed;
}

void ImageWarp::draw()
{
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255, alpha);
    img.setAnchorPercent(0.5, 0.5);
    img.draw(512, 512, img.getWidth() * scale, img.getHeight() * scale); // replace constants
    ofDisableAlphaBlending();
}