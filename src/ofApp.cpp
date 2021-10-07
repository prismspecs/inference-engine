#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup()
{
    // ofSetWindowShape(img_dims * 2, img_dims);
    ofSetWindowShape(img_dims, img_dims);

    // setup Runway
    runway.setup(this, "http://localhost:8000");
    runway.start();

    // setup gui
    if (USE_GUI)
    {
        sliderGroup.setName("sliders");
        for (size_t i{0}; i < num_isolated; ++i)
        {
            ofParameter<float> p;
            std::string name = "vec ";
            name += to_string(i);

            // make sure to randomize vecs because if they are set to 0 it gets funky
            float rand = ofRandom(-1, 1);

            p.set(name, rand, -min_max_vecs, min_max_vecs);
            // p.addListener(this, &ofApp::control_changed);

            vecs.push_back(p);
            sliderGroup.add(vecs.at(i));
        }
        // add listener for the controls so we only update when necessary
        ofAddListener(sliderGroup.parameterChangedE(), this, &ofApp::control_changed);
        gui.setup(sliderGroup);
    }

    ofTrueTypeFontSettings settings("../Ubuntu-B.ttf", 36);
    font_menu.load(settings);

    // set up effects
    origin = ofVec3f(512, 512, -100);
    ofDisableArbTex();
    // ImageWarp iw = ImageWarp(currentImg.getTexture(), origin);
    // warps.push_back(iw);
    ofSetBackgroundColor(0, 0, 0);
    // ofEnableDepthTest();

    // set up graphics contexts
    currentFbo.allocate(img_dims, img_dims, GL_RGB);
    targetFbo.allocate(img_dims, img_dims, GL_RGB);
    surface_main.setup(origin, img_dims);

    // set up game
    // map out the controls
    for (int i = 0; i < 512; i++)
    {
        shuffled_vecs.push_back(i);
    }
    newPosition();

    // controller
    controller.setup(num_vec_groups);
    ofAddListener(controller.headingChange, this, &ofApp::change_heading);
    ofAddListener(controller.moveShip, this, &ofApp::move_ship);
    ofAddListener(controller.sendControls, this, &ofApp::receive_controls);

    

    // // MIDI
    // // print input ports to console
    // midiIn.listInPorts();

    // // open port by number (you may need to change this)
    // //midiIn.openPort(1);
    // //cout << midiIn.getName() << endl;
    // midiIn.openPort("Launch Control XL:Launch Control XL MIDI 1 28:0"); // by name

    // // don't ignore sysex, timing, & active sense messages,
    // // these are ignored by default
    // midiIn.ignoreTypes(false, false, false);

    // // add ofApp as a listener
    // midiIn.addListener(this);

    // // print received messages to the console
    // midiIn.setVerbose(true);
}
//--------------------------------------------------------------
void ofApp::update()
{

    // update controller
    controller.update();

    switch (GAME_STATE)
    {
    case MENU:
    {

        break;
    }
    case PLAYING:
    {

        // update distance
        distance = find_distance(target_position, current_position);

        // they win!
        if (distance < .5)
        {
            GAME_STATE = END;
        }

        if (next_image_loc.size() > 0)
        {

            // check if there is data to receive
            ofxRunwayData dataToReceive;

            while (runway.tryReceive(dataToReceive))
            {
                // are we waiting for a 'current' image?
                if (next_image_loc[0] == CURRENT_IMAGE)
                {
                    dataToReceive.getImage("image", currentImg);
                    currentImg.update();
                    surface_main.update(currentImg.getTexture());
                    // cout << "received new current image" << endl;
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

        // only do this if there has been some change with controls
        if (controls_changed)
        {
            // perhaps only generate an image every X milliseconds
            if (ofGetElapsedTimeMillis() > last_image_gen + image_gen_freq)
            {

                generate_image(current_position, truncation, CURRENT_IMAGE);
                // for some reason target image isnt drawing unless i do this
                // generate_image(target_position, truncation, TARGET_IMAGE);
                last_image_gen = ofGetElapsedTimeMillis();
            }

            if (ofGetElapsedTimeMillis() > last_warp + warp_freq)
            {
                last_warp = ofGetElapsedTimeMillis();
                warp_effect(currentImg.getTexture(), origin);
            }

            controls_changed = false;
        }

        // update effects
        for (int i = warps.size() - 1; i >= 0; i--)
        {
            warps[i].update();

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

        break;
    }
    case END:
    {
        break;
    }
    }
}
//--------------------------------------------------------------
void ofApp::draw()
{

    switch (GAME_STATE)
    {
    case MENU:
    {
        // draw the menu
        ofClear(0);
        if (menu_selection == 0)
        {
            ofSetColor(255, 255, 0);
        }
        font_menu.drawString("start game", 100, 100);

        break;
    }
    case PLAYING:
    {

        // draw image received from Runway
        // if (currentImg.isAllocated())
        // {
        //     ofEnableAlphaBlending();
        //     currentFbo.begin();
        //     ofSetColor(255, 255, 255, 15);
        //     currentImg.draw(0, 0);
        //     currentFbo.end();
        //     ofDisableAlphaBlending();
        //     currentFbo.draw(0, 0);
        // }

        ofEnableAlphaBlending();
        surface_main.draw();
        ofDisableAlphaBlending();

        // ofEnableAlphaBlending();
        // // show zooming images
        // for (int i = warps.size() - 1; i >= 0; i--)
        // {
        //     // cout << "drawing warp" << endl;
        //     warps[i].draw();
        // }
        // ofDisableAlphaBlending();

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

        // draw distance indicator
        ofRectangle r(ofGetWidth() / 2 - 50, ofGetHeight() - 30, 100, 20);
        ofSetColor(0);
        ofDrawRectangle(r);
        string diststr = ofToString(distance);
        ofSetColor(255);
        ofDrawBitmapString(diststr, r.getBottomLeft() + glm::vec3(0, 2, 0));

        ofRectangle distance_bg(10, ofGetHeight() - 40, ofGetWidth() - 20, 20);
        float max_dist = sqrt(4 * num_isolated);
        float mapped_dist = ofMap(distance, max_dist, 0, 0, distance_bg.width);

        ofRectangle distance_meter(10, ofGetHeight() - 40, mapped_dist, 20);
        ofSetColor(0);
        ofDrawRectangle(distance_bg);
        ofSetColor(255);

        ofDrawRectangle(distance_meter);

        if (USE_GUI)
        {
            gui.draw();
        }

        break;
    }
    case END:
    {

        // draw the menu
        ofClear(0);
        drawCenteredText("YOU WIN!", ofColor::yellow);
        break;
    }
    }

    // debug controller
    controller.draw();

    // save frames for video
    if (save_frames)
    {
        ofSaveScreen("frames/" + ofToString(ofGetFrameNum()) + ".png");
    }
}
//--------------------------------------------------------------
void ofApp::newPosition()
{
    // shuffle controller mappings
    ofRandomize(shuffled_vecs);

    // generate a new random starting position taking into account
    // that vectors are now grouped randomly
    
    starting_position = generate_random_grouped_z();
    target_position = generate_random_grouped_z();
    current_position = starting_position;

    ofFile floats("dump.txt", ofFile::WriteOnly);

    for (int i = 0; i < 512; i++)
    {
        floats << ofToString(current_position[i]) << "\t\t\t" << ofToString(target_position[i]) << endl;
    }

    // starting_position.clear();
    // for (int i = 0; i < 512; i++)
    // {
    //     starting_position.push_back(ofRandom(-min_max_vecs, min_max_vecs));
    // }

    // for (int i = 0; i < num_vec_groups; i++)
    // {
    //     for (int v = 0; v < vecs_per_group; v++)
    //     {
    //         // set position but based on the shuffled associated vectors
    //         // cout << shuffled_vecs[i * v] << endl;
    //         starting_position[shuffled_vecs[i * v]] = ofRandom(-min_max_vecs, min_max_vecs);
    //     }
    // }

    // target_position.clear();
    // for (int i = 0; i < 512; i++)
    // {
    //     target_position.push_back(ofRandom(-min_max_vecs, min_max_vecs));
    // }

    // for (int i = 0; i < num_vec_groups; i++)
    // {
    //     for (int v = 0; v < vecs_per_group; v++)
    //     {
    //         // set position but based on the shuffled associated vectors
    //         // cout << shuffled_vecs[i * v] << endl;
    //         target_position[shuffled_vecs[i * v]] = ofRandom(-min_max_vecs, min_max_vecs);
    //     }
    // }

    // // generate a new random starting position
    // starting_position = generate_random_z();
    // // set current position to this new starting position
    // current_position = starting_position;

    // // decide which vectors will be in play this round at random
    // if (RANDOMIZE_VECS)
    // {
    //     isolate_vectors.clear();
    //     vector<int> hat;
    //     for (int i = 0; i < 512; i++)
    //     {
    //         hat.push_back(i);
    //     }

    //     for (int i = 0; i < num_isolated; i++)
    //     {
    //         // select from hat
    //         int picked = int(ofRandom(hat.size()));
    //         isolate_vectors.push_back(hat[picked]);
    //         hat.erase(hat.begin() + picked);
    //     }
    // }
    // else
    // {
    //     // debug, just set vecs directly, no mixing
    //     for (int i = 0; i < num_isolated; i++)
    //     {
    //         isolate_vectors.push_back(i);
    //     }
    // }

    // now that controllable vectors have been isolated for new round, update the
    // current position so that it takes these into effect
    update_position();

    // starting_position should take into account the current controller values
    // starting_position = current_position;

    controls_changed = true;
    generate_image(current_position, truncation, CURRENT_IMAGE);

    // generate a new target based on which input vectors should be frozen
    // target_position = generate_new_target(starting_position, isolate_vectors);

    generate_image(target_position, truncation, TARGET_IMAGE);

    // reset other game vars
    lerp_amount = 0;

    // delete all warp meshes
    warps.clear();
    warp_effect(currentImg.getTexture(), origin);
}
//--------------------------------------------------------------
void ofApp::change_heading(int &av)
{
    active_vec = av;
    controls_changed = true;

    // cout << "received heading change" << endl;
}

//--------------------------------------------------------------
void ofApp::move_ship(float &direction)
{
    // cout << "received move ship" << endl;
    current_position[isolate_vectors[active_vec]] += .01 * direction;
    controls_changed = true;
}
//--------------------------------------------------------------
void ofApp::receive_controls(vector<float> &controls)
{
    // cout << "received controls event" << endl;
    // for (int i = 0; i < controls.size(); i++)
    // {
    //     cout << i << ": " << controls[i] << " | ";
    // }
    // cout << endl;

    // controls are grouped so that the limited number of controls
    // can effect the larger vector of current_position
    // for(int i = 0; i < 512; i++)
    // {
    //     current_position[shuffled_vecs[i]] = 
    // }
    int index = 0;
    for (int i = 0; i < controls.size(); i++)
    {
        // controls is a series of 16 floats
        for (int v = 0; v < vecs_per_group; v++)
        {
            // shuffled_vecs[]
            // cout << index++ << endl;
            // set current position but based on the shuffled associated vectors
            current_position[shuffled_vecs[index]] = controls[i];

            index++;
        }
    }

    // for(int i = 0; i < 512; i+=vecs_per_group) 
    // {
    //     for (int v = 0; v < vecs_per_group; v++)
    //     {
    //         current_position[shuffled_vecs[i+v]] = controls[i];
    //     }
    // }

    controls_changed = true;

    ofFile floats("current_position.txt", ofFile::WriteOnly);

    for (int i = 0; i < 512; i++)
    {
        floats << ofToString(current_position[i]) << endl;
    }
}
//--------------------------------------------------------------
void ofApp::update_position()
{

    for (int i = 0; i < num_isolated; i++)
    {
        // if using GUI sliders...
        // change the associated isolated vector
        // current_position[isolate_vectors[i]] = vecs.at(i);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    // menu
    switch (GAME_STATE)
    {
    case MENU:
        if (key == OF_KEY_UP)
        {
            menu_selection--;
            if (menu_selection < 0)
                menu_selection = menu_count;
        }

        if (key == OF_KEY_DOWN)
        {
            menu_selection++;
            if (menu_selection > menu_count)
                menu_selection = 0;
        }

        if (key == ' ')
        {
            GAME_STATE = PLAYING;
        }
        break;

    case PLAYING:
    {

        if (key == 'n')
        {
            // reset game
            newPosition();
        }

        // playing
        if (key == 'w')
        {
            warp_effect(currentImg.getTexture(), origin);
        }

        if (key == 't')
        {
            targetimg_maximized = !targetimg_maximized;
        }

        break;
    }
    }

    if (key == 's')
    {
        save_frames = !save_frames;
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
        lerp_ship();
    }
}
//--------------------------------------------------------------
void ofApp::lerp_ship()
{
    // move all sliders to correct values slowly
    for (size_t i{0}; i < num_isolated; ++i)
    {
        float lerped = ofLerp(starting_position[isolate_vectors[i]], target_position[isolate_vectors[i]], lerp_amount);
        vecs.at(i).set(lerped);
    }

    lerp_amount += lerp_speed;
    lerp_amount = ofClamp(lerp_amount, 0, 1);

    generate_image(current_position, truncation, CURRENT_IMAGE);
}
//--------------------------------------------------------------
void ofApp::control_changed(ofAbstractParameter &e)
{

    update_position();
    controls_changed = true;

    // probably shouldnt generate a new image every time the controls change
    // because they work faster than the framerate
    // generate_image(current_position, truncation, CURRENT_IMAGE);
}
//--------------------------------------------------------------
void ofApp::warp_effect(ofTexture &texture, ofVec3f location)
{
    // image warp
    ImageWarp iw = ImageWarp(texture, location);
    warps.push_back(iw);

    // find the oldest warp that isn't yet flying
    warps[warps.size() - 2].fly = true;
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
vector<float> ofApp::generate_random_grouped_z()
{
    vector<float> z; // create an empty vector of floats
    for (int i = 0; i < 512; i++)
    {
        z.push_back(0.0);
    }

    // for (int i = 0; i < num_vec_groups; i++)
    // {
    //     for (int v = 0; v < vecs_per_group; v++)
    //     {
    //         // set position but based on the shuffled associated vectors
    //         // cout << shuffled_vecs[i * v] << endl;
    //         z[shuffled_vecs[i * v]] = ofRandom(-min_max_vecs, min_max_vecs);
    //     }
    // }

    int index = 0;
    for (int i = 0; i < num_vec_groups; i++)
    {
        float rand = ofRandom(-min_max_vecs, min_max_vecs);
        // controls is a series of 16 floats
        for (int v = 0; v < vecs_per_group; v++)
        {
            z[shuffled_vecs[index]] = rand;
            index++;
        }
    }

    return z;
}
//--------------------------------------------------------------
void ofApp::generate_image(vector<float> z, float truncation, int next_loc)
{

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

//--------------------------------------------------------------
void ofApp::drawCenteredText(string str, ofColor color)
{
    ofSetColor(color);
    font_menu.drawString(str, ofGetWidth() / 2 - font_menu.stringWidth(str) / 2, ofGetHeight() / 2 - font_menu.stringHeight(str) / 2);
}