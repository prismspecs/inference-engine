#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup()
{
    //uncomment the following line if you want a verbose log (which means a lot of info will be printed)
    // ofSetLogLevel(OF_LOG_VERBOSE);

    // ofSetWindowShape(img_dims * 2, img_dims);

    ofSetWindowShape(img_dims, img_dims);

    // setup Runway
    runway.setup(this, "http://localhost:8000");
    runway.start();

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

    // set up game
    newPosition();

    // set up graphics contexts
    currentFbo.allocate(img_dims, img_dims, GL_RGB);
    targetFbo.allocate(img_dims, img_dims, GL_RGB);

    //targetImg.allocate(1024,1024,OF_IMAGE_COLOR);

    // MIDI
    // print input ports to console
    midiIn.listInPorts();

    // open port by number (you may need to change this)
    //midiIn.openPort(1);
    //cout << midiIn.getName() << endl;
    midiIn.openPort("Launch Control XL:Launch Control XL MIDI 1 28:0"); // by name

    // don't ignore sysex, timing, & active sense messages,
    // these are ignored by default
    midiIn.ignoreTypes(false, false, false);

    // add ofApp as a listener
    midiIn.addListener(this);

    // print received messages to the console
    midiIn.setVerbose(true);
}
//--------------------------------------------------------------
void ofApp::update()
{
    // update distance
    distance = find_distance(target_position, current_position);

    // debugging the image request stack
    // for (int i = 0; i < next_image_loc.size(); i++)
    // {
    //     cout << i << ", " << next_image_loc[i] << endl;
    // }

    if (next_image_loc.size() > 0)
    {

        // check if there is data to receive
        ofxRunwayData dataToReceive;
        // bool is_data = runway.tryReceive(dataToReceive);
        // cout << "is there data? " << is_data << endl;

        while (runway.tryReceive(dataToReceive))
        {
            // are we waiting for a 'current' image?
            if (next_image_loc[0] == CURRENT_IMAGE)
            {
                dataToReceive.getImage("image", currentImg);
                currentImg.update();

                // // image warp
                // ImageWarp iw = ImageWarp();
                // warps.push_back(iw);
                // ofImage copy = currentImg;
                // iw.setup(copy);
            }
            // are we waiting for a 'target' image?
            else if (next_image_loc[0] == TARGET_IMAGE)
            {
                // cout << "placing target" << endl;

                dataToReceive.getImage("image", targetImg);
                targetImg.update();
            }

            // erase this entry
            next_image_loc.erase(next_image_loc.begin());
        }
    }

    // perhaps only generate an image every X milliseconds
    // should also prevent duplicates...
    if (ofGetElapsedTimeMillis() > last_image_gen + image_gen_freq)
    {
        generate_image(current_position, truncation, CURRENT_IMAGE);
        // for some reason target image isnt drawing unless i do this
        generate_image(target_position, truncation, TARGET_IMAGE);
        last_image_gen = ofGetElapsedTimeMillis();
    }

    // update effects
    // cout << warps.size() << endl;
    for (int i = warps.size() - 1; i >= 0; i--)
    {
        warps[i].update();

        // cout << warps[i].alpha << endl;

        if (warps[i].alpha <= 0)
        {
            warps.erase(warps.begin() + i);
        }
    }

    // HUD

    // MIDI
    for (unsigned int i = 0; i < midiMessages.size(); ++i)
    {
        ofxMidiMessage &message = midiMessages[i];

        int c = message.control - 1;

        if (c < num_isolated)
        {
            vecs.at(c) = ofMap(message.value, 0, 127, -min_max_vecs, min_max_vecs);
            update_position();
        }
    }
}
//--------------------------------------------------------------
void ofApp::draw()
{

    // draw image received from Runway
    if (currentImg.isAllocated())
    {
        ofEnableAlphaBlending();
        currentFbo.begin();
        ofSetColor(255, 255, 255, 15);
        currentImg.draw(0, 0);
        currentFbo.end();
        ofDisableAlphaBlending();

        currentFbo.draw(0, 0);

        // make_mesh(currentImg).draw();
        // draw_stars(currentImg);
    }

    // show zooming images
    for (int i = 0; i < warps.size(); i++)
    {
        // cout << "drawing warp" << endl;
        warps[i].draw();
    }

    // draw target location image
    if (targetImg.isAllocated())
    {
        //targetImg.draw(img_dims, 0);

        // draw to fbo instead and integrate into HUD
        targetFbo.begin();
        targetImg.draw(0, 0);
        targetFbo.end();

        // draw the target image, allow toggle for larger view
        if (targetimg_maximized)
        {
            targetimg_x = ofLerp(targetimg_x, tiX_max, .05);
            targetimg_y = ofLerp(targetimg_y, tiY_max, .05);
            targetimg_dim = ofLerp(targetimg_dim, tiD_max, .05);

            targetFbo.draw(targetimg_x, targetimg_y, targetimg_dim, targetimg_dim);
        }
        else
        {

            targetimg_x = ofLerp(targetimg_x, tiX_min, .05);
            targetimg_y = ofLerp(targetimg_y, tiY_min, .05);
            targetimg_dim = ofLerp(targetimg_dim, tiD_min, .05);

            targetFbo.draw(targetimg_x, targetimg_y, targetimg_dim, targetimg_dim);
        }
    }

    // draw runway's status. It returns the bounding box of the drawn text. It is useful so you can draw other stuff and avoid overlays
    // ofRectangle r = runway.drawStatus(620, 440, true);
    // ofDrawBitmapString("Press ' ' to send to Runway", r.getBottomLeft() + glm::vec3(0, 20, 0));

    // draw distance
    ofRectangle r(ofGetWidth() / 2 - 50, ofGetHeight() - 30, 100, 20);
    ofSetColor(0);
    ofDrawRectangle(r);
    string diststr = ofToString(distance);
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
void ofApp::newPosition()
{
    starting_position = generate_random_z();
    current_position = starting_position;

    // decide which vectors will be in play this round
    isolate_vectors.clear();
    vector<int> hat;
    for (int i = 0; i < 512; i++)
    {
        hat.push_back(i);
    }

    // cout << "isolated vectors are ";

    for (int i = 0; i < num_isolated; i++)
    {
        // select from hat
        int picked = int(ofRandom(hat.size()));
        isolate_vectors.push_back(hat[picked]);
        hat.erase(hat.begin() + picked);

        // cout << picked << ", ";
    }

    // now that controllable vectors have been isolated for new round, update the
    // current position so that it takes these into effect
    update_position();

    // cout << endl;

    // generate a new target based on which input vectors should be frozen
    target_position = generate_new_target(starting_position, isolate_vectors);

    bWaitingForTarget = true;

    generate_image(target_position, truncation, TARGET_IMAGE);

    // create image for destination
    // runway.get("image", targetImg);
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

    if (key == ' ')
    {
        generate_image(generate_random_z(), truncation, CURRENT_IMAGE);
    }

    if (key == 'w')
    {
        // image warp
        ImageWarp iw = ImageWarp();
        warps.push_back(iw);
        ofImage copy = currentImg;
        iw.setup(copy);
    }

    if (key == 't')
    {
        targetimg_maximized = !targetimg_maximized;
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

    if (key == 'x')
    {
        // move all sliders to correct values immediately
        for (size_t i{0}; i < num_isolated; ++i)
        {
            vecs.at(i).set(target_position[isolate_vectors[i]]);
        }

        generate_image(current_position, truncation, CURRENT_IMAGE);
    }

    if (key == 'c')
    {
        // move all sliders to correct values slowly
        for (size_t i{0}; i < num_isolated; ++i)
        {
            float lerped = ofLerp(vecs.at(i), target_position[isolate_vectors[i]], .02);
            vecs.at(i).set(lerped);
        }

        generate_image(current_position, truncation, CURRENT_IMAGE);
    }

    if (key == 'n')
    {
        // reset game
        newPosition();
    }
}
//--------------------------------------------------------------
void ofApp::control_changed(ofAbstractParameter &e)
{

    update_position();

    // probably shouldnt generate a new image every time the controls change
    // because they work faster than the framerate
    // generate_image(current_position, truncation, CURRENT_IMAGE);

    //cout << find_distance(target_position, current_position) << endl;
}

//--------------------------------------------------------------
void ofApp::update_position()
{
    // ImageWarp iw = ImageWarp();
    // warps.push_back(iw);
    // ofImage copy = currentImg;
    // iw.setup(copy);

    for (size_t i{0}; i < num_isolated; ++i)
    {
        // change the associated isolated vector
        current_position[isolate_vectors[i]] = vecs.at(i);
        //current_position[i] = vecs.at(i); // OOPS! this was wrong
    }
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
void ofApp::generate_image(vector<float> z, float truncation, int next_loc)
{

    // cout << "generating image " << counter++ << endl;

    // skip if content image isn't loaded yet

    if (runway.isBusy())
    {
        return;
        cout << "runway was busy" << endl;
    }

    ofxRunwayData data;
    data.setFloats("z", z);
    data.setFloat("truncation", truncation);

    runway.send(data);

    bWaitingForResponse = true;

    // add 'where this image will go' to the stack so it gets placed properly later
    next_image_loc.push_back(next_loc);
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
//--------------------------------------------------------------
ofMesh ofApp::make_mesh(ofImage image)
{
    ofMesh mesh;

    mesh.setMode(OF_PRIMITIVE_POINTS);
    mesh.enableColors();

    float intensityThreshold = 0.0;

    int w = image.getWidth();
    int h = image.getHeight();

    for (int x = 0; x < w; ++x)
    {
        for (int y = 0; y < h; ++y)
        {
            ofColor c = image.getColor(x, y);
            float intensity = c.getLightness();
            if (intensity >= intensityThreshold)
            {
                ofVec3f pos(x, y, intensity * .1 * distance);
                mesh.addVertex(pos);
                // When addColor(...), the mesh will automatically convert
                // the ofColor to an ofFloatColor
                mesh.addColor(c);
            }
        }
    }

    return mesh;
}
//--------------------------------------------------------------
void ofApp::draw_stars(ofImage image)
{

    int w = image.getWidth();
    int h = image.getHeight();

    for (int x = 0; x < w; x += 21)
    {
        for (int y = 0; y < h; y += 21)
        {
            ofColor c = image.getColor(x, y);
            ofSetColor(c);

            float phase = sin(ofGetElapsedTimeMillis() * .0001 + x * .01);
            float n = ofNoise(x + ofGetElapsedTimeMillis() * .0001, y + ofGetElapsedTimeMillis() * .0001);

            ofVec3f pos(x, y, n * 100);

            ofDrawRectangle(pos, 3, 3);
        }
    }
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
//--------------------------------------------------------------
void ofApp::exit()
{

    // clean up
    midiIn.closePort();
    midiIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage &msg)
{

    // add the latest message to the message queue
    midiMessages.push_back(msg);

    // remove any old messages if we have too many
    while (midiMessages.size() > maxMessages)
    {
        midiMessages.erase(midiMessages.begin());
    }
}