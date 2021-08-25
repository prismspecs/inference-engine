#include "imgwarp.h"

ImageWarp::ImageWarp()
{
    img.allocate(1024, 1024, OF_IMAGE_COLOR);
}

void ImageWarp::setup(ofImage image)
{
    img = image;
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