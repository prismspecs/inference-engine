#include "controller.h"
#include "ofxGLFWJoystick.h"

Controller::Controller()
{
}

void Controller::setup()
{
    ofxGLFWJoystick::one().printJoystickList();
}

void Controller::update()
{
    ofxGLFWJoystick::one().update();

    float joyX = ofxGLFWJoystick::one().getAxisValue(0, 0);
    float joyY = ofxGLFWJoystick::one().getAxisValue(1, 0);

    double rads = getAngle(ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2), ofVec2f(joyX, joyY));
    float degs = ofRadToDeg(rads);
    cout << rads << ", " << degs << endl;

    active_vec = ofMap(rads, 0, TWO_PI, 0, 8); // assume x controllable vectors

    // stopped here... make a circular selection menu so player can having a heading
    // in the direction of a vector
    
    // cout << active_vec << endl;
}

void Controller::draw()
{
    ofxGLFWJoystick::one().drawDebug(100, 100);
    int joystickID = 0;
    float joyX = ofxGLFWJoystick::one().getAxisValue(0, joystickID);
    float joyY = ofxGLFWJoystick::one().getAxisValue(1, joystickID);

    //lets map the joystick to our window size
    float mappedX = ofMap(joyX, -1, 1, 0, ofGetWidth());
    float mappedY = ofMap(joyY, -1, 1, 0, ofGetHeight());

    ofSetColor(255);
    ofCircle(mappedX, mappedY, 3);
}

double Controller::getAngleRads(ofVec2f a, ofVec2f b)
{
    float delta_x = (a.x + b.x) - (a.x);
    float delta_y = (a.y + b.y) - (a.y);
    float theta_radians = atan2(delta_y, delta_x) + PI/2;

    // we need to map to coord system when 0 degree is at 3 O'clock, 270 at 12 O'clock
    if (theta_radians < 0)
        theta_radians = abs(theta_radians);
    else
        theta_radians = 2 * PI - theta_radians;

    return theta_radians;
}
