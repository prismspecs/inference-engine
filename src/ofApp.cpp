#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup()
{
    //uncomment the following line if you want a verbose log (which means a lot of info will be printed)
    // ofSetLogLevel(OF_LOG_VERBOSE);

    ofSetWindowShape(1024, 512);

    // setup Runway
    runway.setup(this, "http://localhost:8000");
    runway.start();

    // set up game
    starting_position = generate_random_z();
    current_position = starting_position;

    // decide which vectors will be in play this round
    isolate_vectors.clear();
    vector<int> hat;
    for (int i = 0; i < 512; i++)
    {
        hat.push_back(i);
    }

    for (int i = 0; i < num_isolated; i++)
    {
        // select from hat
        int picked = int(ofRandom(hat.size()));
        isolate_vectors.push_back(hat[picked]);
        hat.erase(hat.begin() + picked);

        cout << picked;
    }

    cout << endl;

    // generate a new target based on which input vectors should be frozen
    target_position = generate_new_target(starting_position, isolate_vectors);

    generate_image(target_position, truncation);

    // create image for destination
    runway.get("image", targetImg);
    bWaitingForTarget = true;

    // setup gui
    sliderGroup.setName("sliders");
    for (size_t i{0}; i < num_isolated; ++i)
    {
        ofParameter<float> p;
        std::string name = "vec ";
        name += to_string(i);
        p.set(name, 0, -min_max_vecs, min_max_vecs);
        // p.addListener(this, &ofApp::control_changed);

        vecs.push_back(p);
        sliderGroup.add(vecs.at(i));
        
    }
    // add listener for the controls so we only update when necessary
    ofAddListener(sliderGroup.parameterChangedE(), this, &ofApp::control_changed);
    gui.setup(sliderGroup);
}
//--------------------------------------------------------------
void ofApp::update()
{
    if (bWaitingForTarget)
    {
        runway.get("image", targetImg);

        if (targetImg.isAllocated())
            bWaitingForTarget = false;
    }
    if (!bWaitingForTarget && bWaitingForResponse)
    {
        runway.get("image", currentImg);
        bWaitingForResponse = false;
    }

    // using GUI
    // vector<float> randZ = starting_position;
}
//--------------------------------------------------------------
void ofApp::draw()
{
    // draw target location image
    // draw image received from Runway
    if (targetImg.isAllocated())
    {
        targetImg.draw(512, 0);
    }

    // draw image received from Runway
    if (currentImg.isAllocated())
    {
        currentImg.draw(0, 0);
    }

    // draw runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
    // ofRectangle r = runway.drawStatus(620, 440, true);
    // ofDrawBitmapString("Press ' ' to send to Runway", r.getBottomLeft() + glm::vec3(0, 20, 0));

    // draw distance
    ofRectangle r(ofGetWidth() / 2 - 50, ofGetHeight() - 30, 100, 20);
    ofSetColor(0);
    ofDrawRectangle(r);
    string diststr = ofToString(find_distance(target_position, current_position));
    ofSetColor(255);
    ofDrawBitmapString(diststr, r.getBottomLeft() + glm::vec3(0, 2, 0));

    // ofDrawBitmapString("Press ' ' to send to Runway", r.getBottomLeft() + glm::vec3(0, 20, 0));

    gui.draw();

    // save frame
    // string fn = ofToString(ofGetFrameNum(),4,'0')+".png";
    // cout << fn << endl;

    // ofSaveScreen(fn);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

    if (key == ' ')
    {
        generate_image(generate_random_z(), truncation);
    }
}
//--------------------------------------------------------------
void ofApp::control_changed(ofAbstractParameter& e)
{
    for (size_t i{0}; i < num_isolated; ++i)
    {
        current_position[i] = vecs.at(i);
    }

    generate_image(current_position, truncation);

    cout << find_distance(target_position, current_position) << endl;
}

//--------------------------------------------------------------
vector<float> ofApp::generate_random_z()
{

    vector<float> z; // create an empty vector of floats
    for (int i = 0; i < 512; i++)
    {
        z.push_back(ofRandom(-min_max_vecs, min_max_vecs));
    }

    return z;
}

//--------------------------------------------------------------
void ofApp::generate_image(vector<float> z, float truncation)
{
    // skip if content image isn't loaded yet

    if (runway.isBusy())
        return;

    ofxRunwayData data;
    data.setFloats("z", z);
    data.setFloat("truncation", truncation);

    runway.send(data);

    bWaitingForResponse = true;
}
//--------------------------------------------------------------
vector<float> ofApp::generate_new_target(vector<float> startPos, vector<int> iso_vecs)
{
    vector<float> target = startPos;

    // target position should only randomize the isolated controllable vectors
    for (int i = 0; i < iso_vecs.size(); i++)
    {
        target[iso_vecs[i]] = ofRandom(-min_max_vecs, min_max_vecs);
    }

    return target;
}
//--------------------------------------------------------------
float ofApp::find_distance(vector<float> a, vector<float> b)
{
    float total_diff = 0;
    for (int i = 0; i < a.size(); i++)
    {
        float difference = pow(abs(a[i] - b[i]), 2);
        total_diff += difference;
    }

    float root_diff = sqrt(total_diff);

    return root_diff;
}
// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson &info)
{
    ofLogNotice("ofApp::runwayInfoEvent") << info.dump(2);
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string &message)
{
    ofLogNotice("ofApp::runwayErrorEvent") << message;
}
