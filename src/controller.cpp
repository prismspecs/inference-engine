#include "controller.h"

Controller::Controller()
{
}

void Controller::setup(int ncv, float vectorspeed)
{
    num_control_vecs = ncv;
    vector_speed = vectorspeed;
    // so i can draw a grid of controls
    num_controls_root = sqrt(num_control_vecs);

    for (int i = 0; i < num_control_vecs; i++)
    {
        controls.push_back(ofRandom(-1, 1));
        target_controls.push_back(ofRandom(-1, 1));
    }

    ofAddListener(ofEvents().keyPressed, this, &Controller::keyPressed);
    ofAddListener(ofEvents().keyReleased, this, &Controller::keyReleased);

    // set up vector readouts
    int i = 0;
    for (int y = 0; y < num_controls_root; y++)
    {
        for (int x = 0; x < num_controls_root; x++)
        {
            float _x = controls_start_x + (x * control_dim_x) + control_dim_x / 2;
            float _y = controls_start_y + (y * control_dim_y) + control_dim_y / 2;

            VecDisplay v;
            v.setup(_x, _y, control_dim_x * .4);
            vec_displays.push_back(v);
        }
    }
}

void Controller::reset(vector<float> current_pos, vector<float> target_pos, vector<int> shuffled_vecs)
{
    // set the controller's initial position to the randomly generated
    // starting position of the player

    // incoming 512 floats, shuffled
    // need to unshuffle then i should see groups of 32 floats w same values
    vector<float> unshuffled_controls;
    // also for target
    vector<float> unshuffled_targets;

    // in unshuffled form they are grouped (as of now) in 32 floats
    // so skip every 32 and add that to a vector, then map that onto the current controls
    int controls_per_group = 512 / num_control_vecs;
    for (int i = 0; i < current_pos.size(); i += controls_per_group)
    {
        unshuffled_controls.push_back(current_pos[shuffled_vecs[i]]);
        unshuffled_targets.push_back(target_pos[shuffled_vecs[i]]);
    }
    for (int i = 0; i < controls.size(); i++)
    {
        controls[i] = unshuffled_controls[i];
        target_controls[i] = unshuffled_targets[i];
    }

    lerp_amount = 0;
}
void Controller::receive_serial(int &inByte)
{
    if (inByte == 0)
    {
        controls[active_vec] -= vector_speed;
        ofNotifyEvent(sendControlVectors, controls);
    }
    if (inByte == 1)
    {
        controls[active_vec] += vector_speed;
        ofNotifyEvent(sendControlVectors, controls);
    }
}
void Controller::update()
{

    for (int i = 0; i < controls.size(); i++)
    {
        // draw current position
        float diff = abs(controls[i] - target_controls[i]);
        vec_displays[i].diff = diff;

        if (i == active_vec)
        {
            vec_displays[i].active = true;
        }
        else
        {
            vec_displays[i].active = false;
        }

        vec_displays[i].update();
    }
}

void Controller::lerp_ship()
{
    for (int i = 0; i < controls.size(); i++)
    {
        float lerped = ofLerp(controls[i], target_controls[i], lerp_amount);
        controls[i] = lerped;
    }
    ofNotifyEvent(sendControlVectors, controls);
    lerp_amount += lerp_speed;
    lerp_amount = ofClamp(lerp_amount, 0, 1);
}

void Controller::keyReleased(ofKeyEventArgs &e)
{
    if (e.key == OF_KEY_LEFT)
    {
        string button = "left";
        ofNotifyEvent(buttonPress, button);
        active_vec--;
        if (active_vec < 0)
            active_vec = num_control_vecs - 1;

        ofNotifyEvent(activeVecChanged, active_vec);

    }

    if (e.key == OF_KEY_RIGHT)
    {
        string button = "right";
        ofNotifyEvent(buttonPress, button);
        active_vec += 1;
        if (active_vec >= num_control_vecs)
            active_vec = 0;

        ofNotifyEvent(activeVecChanged, active_vec);
    }

    if (e.key == OF_KEY_UP)
    {
        string button = "up";
        ofNotifyEvent(buttonPress, button);
    }

    if (e.key == OF_KEY_DOWN)
    {
        string button = "down";
        ofNotifyEvent(buttonPress, button);
    }

    if (e.key == ' ')
    {
        string button = "fire";
        ofNotifyEvent(buttonPress, button);
    }
}

void Controller::keyPressed(ofKeyEventArgs &e)
{

    if (e.key == OF_KEY_UP)
    {
        controls[active_vec] += vector_speed;
        ofNotifyEvent(sendControlVectors, controls);
    }

    if (e.key == OF_KEY_DOWN)
    {
        controls[active_vec] -= vector_speed;
        ofNotifyEvent(sendControlVectors, controls);
    }
}

void Controller::draw()
{

    // int i = 0;
    // for (int y = 0; y < num_controls_root; y++)
    // {
    //     for (int x = 0; x < num_controls_root; x++)
    //     {
    //         float _x = controls_start_x + (x * control_dim_x) + control_dim_x / 2;
    //         float _y = controls_start_y + (y * control_dim_y) + control_dim_y / 2;

    //         ofSetColor(255, 255, 0);
    //         ofCircle(_x , _y , control_dim_x *.4);
    //         // ofRect(_x, _y, control_dim_x, control_dim_y);

    //         // draw current position
    //         float diff = abs(controls[i] - target_controls[i]);
    //         ofSetColor(0);
    //         ofDrawBitmapString(controls[i], _x, _y);
    //         ofSetColor(255, 0, 0);
    //         ofDrawBitmapString(ofToString(diff,2), _x, _y + 12);

    //         i++;
    //     }
    // }
    ofSetCircleResolution(12);
    ofNoFill();

    for (int i = 0; i < vec_displays.size(); i++)
    {
        vec_displays[i].draw();
    }
}

double Controller::getAngleRads(ofVec2f a, ofVec2f b)
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
