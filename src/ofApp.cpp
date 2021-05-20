#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup()
{
    //uncomment the following line if you want a verbose log (which means a lot of info will be printed)
    ofSetLogLevel(OF_LOG_VERBOSE);

    ofSetWindowShape(1024, 512);

    // setup Runway
    runway.setup(this, "http://localhost:8000");
    runway.start();

    // set up game
    starting_position = generate_random_z();
    target_position = generate_random_z();

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

    generate_image(target_position, truncation);
    
    // create image for destination
    runway.get("image", targetImg);
    bWaitingForTarget = true;
    


    // setup gui
    sliderGroup.setName("sliders");
    sliderGroup.add(vec0.set("vec 0", 0, -5, 5));
    sliderGroup.add(vec1.set("vec 1", 0, -5, 5));
    sliderGroup.add(vec2.set("vec 2", 0, -5, 5));
    sliderGroup.add(vec3.set("vec 3", 0, -5, 5));
    sliderGroup.add(vec4.set("vec 4", 0, -5, 5));
    sliderGroup.add(vec5.set("vec 5", 0, -5, 5));
    sliderGroup.add(vec6.set("vec 6", 0, -5, 5));
    sliderGroup.add(vec7.set("vec 7", 0, -5, 5));
    sliderGroup.add(vec8.set("vec 8", 0, -5, 5));
    sliderGroup.add(vec9.set("vec 9", 0, -5, 5));
    // sliderGroup.add(floatSlider[0].set("float slider", 32, 32, 256));

    gui.setup(sliderGroup);
}
//--------------------------------------------------------------
void ofApp::update()
{
    if(bWaitingForTarget)
    {
        runway.get("image", targetImg);

        if(targetImg.isAllocated())
            bWaitingForTarget = false;
    }
    if (!bWaitingForTarget && bWaitingForResponse)
    {
        runway.get("image", currentImg);
        bWaitingForResponse = false;
    }

    // using GUI
    vector<float> randZ = starting_position;
    randZ[0] = vec0;
    randZ[1] = vec1;
    randZ[2] = vec2;
    randZ[3] = vec3;
    randZ[4] = vec4;
    randZ[5] = vec5;
    randZ[6] = vec6;
    randZ[7] = vec7;
    randZ[8] = vec8;
    randZ[9] = vec9;

    generate_image(randZ, truncation);

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

    gui.draw();

    // save frame
    string fn = ofToString(ofGetFrameNum(),4,'0')+".png";
    cout << fn << endl;

    ofSaveScreen(fn);
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
vector<float> ofApp::generate_random_z()
{

    vector<float> z; // create an empty vector of floats
    for (int i = 0; i < 512; i++)
    {
        z.push_back(ofRandom(-1, 1));
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
