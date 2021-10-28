#include "vector_display.h"

VecDisplay::VecDisplay()
{
}

void VecDisplay::setup(float _x, float _y, float _dim)
{
    pos = ofVec2f(_x, _y);
    dim = _dim;

    phase = ofRandom(10);
}

void VecDisplay::update()
{
    // some color change or something to emphasize
    // how far from correct vector position
    float adjusted_diff = ofClamp(diff, 0.2, .99);

    color = ofColor(adjusted_diff * 255, 1 - adjusted_diff * 255, 50);

    phase_speed = ofMap(abs(diff), 0, 1, 0.01, max_phase_speed);
    phase_speed = ofClamp(phase_speed, 0, 1);
    phase += phase_speed;

    phase_amplitude = adjusted_diff * max_phase_amplitude;
}

void VecDisplay::draw()
{

    // some effect to emphasize active vector
    if (active)
    {
        ofSetColor(color);
        ofNoFill();
        ofDrawRectangle(pos.x - dim, pos.y - dim, dim * 2, dim * 2);
    }

    ofSetColor(color);
    ofSetLineWidth(2);

    // render the different as a text string
    // ofSetColor(255, 255, 0);
    // ofDrawBitmapString(ofToString(diff, 2), pos.x, pos.y + 12);

    // sine wave graph
    float px, py;
    px = pos.x - dim;
    py = pos.y + sin(phase + (0 * .2)) * phase_amplitude;

    ofSetLineWidth(1);

    for (int i = 1; i < wave_resolution; i++)
    {
        float x = ofMap(i, 0, wave_resolution, pos.x - dim, pos.x + dim);
        // give it a little random offset based on the difference from target
        // to emphasize that the control is out of tune
        float y = pos.y + sin(phase + (i * .2)) * phase_amplitude + ofRandom(diff * -10, diff * 10);

        ofDrawLine(x, y, px, py);

        px = x;
        py = y;

    }
}

double VecDisplay::getAngleRads(ofVec2f a, ofVec2f b)
{
    float delta_x = (a.x + b.x) - (a.x);
    float delta_y = (a.y + b.y) - (a.y);
    float theta_radians = atan2(delta_y, delta_x) + PI / 2;

    // we need to map to coord system when 0 degree is at 3 O'clock, 270 at 12 O'clock
    if (theta_radians < 0)
        theta_radians = abs(theta_radians);
    else
        theta_radians = 2 * PI - theta_radians;

    return theta_radians;
}
