#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup()
{

    // GRAPHICS SETUP
    // ofSetWindowShape(img_dims, img_dims);
    ofSetBackgroundAuto(false);
    ofDisableArbTex(); // important for texture mapping
    ofHideCursor();

    ofTrueTypeFontSettings settings("fonts/ContrailOne-Regular.ttf", 48);
    font_menu.load(settings);

    // serial
    serial.listDevices();
    vector<ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    serial.setup(0, 115200);
    // std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();

    // ofLogNotice("ofApp::setup") << "Connected Devices: ";

    // for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    // {
    //     ofLogNotice("ofApp::setup") << "\t" << devicesInfo[i];
    // }

    // if (!devicesInfo.empty())
    // {
    //     // Connect to the first matching device.
    //     bool success = serial.setup(devicesInfo[0], 115200);

    //     if (success)
    //     {
    //         ofLogNotice("ofApp::setup") << "Successfully setup " << devicesInfo[0];
    //     }
    //     else
    //     {
    //         ofLogNotice("ofApp::setup") << "Unable to setup " << devicesInfo[0];
    //     }
    // }
    // else
    // {
    //     ofLogNotice("ofApp::setup") << "No devices connected.";
    // }

    // set up effects
    // origin = ofVec3f(512, 512, -100);
    origin = ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0);

    // ImageWarp iw = ImageWarp(currentImg.getTexture(), origin);
    // warps.push_back(iw);
    // ofSetBackgroundColor(0, 0, 0);
    // ofEnableDepthTest();

    // set up graphics contexts
    currentFbo.allocate(img_dims, img_dims, GL_RGB);
    targetFbo.allocate(img_dims, img_dims, GL_RGB);
    surface_main.setup(origin, img_dims);

    // sound
    sound.setup();
    sound.menu_start();

    // setup Runway
    runway.setup(this, "http://localhost:9000");
    runway.start();

    // setup tutorial
    tutorial_vid.load("videos/tutorial_nosound.mp4");
    tutorial_vid.setLoopState(OF_LOOP_NONE);

    // set up game
    // map out the controls
    for (int i = 0; i < 512; i++)
    {
        shuffled_vecs.push_back(i);
    }

    // controller
    controller.setup(num_vec_groups, global_vector_speed);
    ofAddListener(controller.sendControlVectors, this, &ofApp::receive_control_vectors);
    ofAddListener(controller.buttonPress, this, &ofApp::receive_button);
    ofAddListener(controller.activeVecChanged, this, &ofApp::change_active_vec);

    newPosition();
}
//--------------------------------------------------------------
void ofApp::update()
{

    // read serial
    int myByte = -1;
    myByte = serial.readByte();
    if (myByte == OF_SERIAL_NO_DATA)
    {
    }
    else if (myByte == OF_SERIAL_ERROR)
    {
    }
    else
    {
        // cout << "serial received " << myByte << endl;
        controller.receive_serial(myByte);
    }

    // update controller
    controller.update();

    // update sound
    sound.update();

    switch (GAME_STATE)
    {
    case MENU:
    {
        if (menu_startfade)
        {
            if (ofGetElapsedTimeMillis() > menu_fadestartedtime + menu_fadetime)
            {
                GAME_STATE = PLAYING;
                menu_startfade = false;
                game_startfade = true; // help to cover up loading
                game_fadestartedtime = ofGetElapsedTimeMillis();
                newPosition();

                // end menu music loop, begin game music loop
                sound.menu_end();
                sound.game_start();

                last_time_controls_changed = ofGetElapsedTimeMillis();
            }
        }
        break;
    }
    case PLAYING:
    {

        if (game_startfade)
        {
            if (ofGetElapsedTimeMillis() > game_fadestartedtime + game_fadetime)
            {
                game_startfade = false;
            }
        }

        // update distance
        distance = find_distance(target_position, current_position);

        // send distance to sound engine
        sound.set_proximity(distance, max_dist);

        // if game times out
        if (ofGetElapsedTimeMillis() > last_time_controls_changed + controls_idle_timeout)
        {
            // end game music loop
            sound.game_end();
            sound.menu_start();
            GAME_STATE = MENU;
            end_startfade = false;
        }

        // VICTORY CONDITION
        if (distance < min_distance)
        {
            GAME_STATE = END;
            end_startfade = true;
            end_fadestartedtime = ofGetElapsedTimeMillis();
            sound.play_once("victory");

            // end game music loop
            sound.game_end();
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

            last_time_controls_changed = ofGetElapsedTimeMillis();

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

        break;
    }
    case END:
    {
        if (ofGetElapsedTimeMillis() > end_fadestartedtime + end_fadetime)
        {
            GAME_STATE = MENU;
            end_startfade = false;
            // end game music loop
            sound.menu_start();
        }
        break;
    }

    case HELP_SCREEN:
    {

        // play tutorial vid
        tutorial_vid.update();

        if (tutorial_vid.getPosition() >= 0.98)
        {
            GAME_STATE = PLAYING;
        }
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

        float op = 255;
        ofClear(0);

        // draw the menu
        if (menu_startfade)
        {
            // fade out over time
            op = ofMap(ofGetElapsedTimeMillis(), menu_fadestartedtime,
                       menu_fadestartedtime + menu_fadetime, 255, 0);
        }

        if (menu_selection == 0)
        {
            ofSetColor(255, 255, 50, op);

            font_menu.drawString("start game", ofGetWidth() / 2 - 285 / 2, ofGetHeight() / 2);

            ofSetColor(50, 50, 50, op);
            font_menu.drawString("how to play", ofGetWidth() / 2 - 299 / 2, ofGetHeight() / 2 + 80);
        }

        if (menu_selection == 1)
        {
            ofSetColor(50, 50, 50, op);
            font_menu.drawString("start game", ofGetWidth() / 2 - 285 / 2, ofGetHeight() / 2);

            ofSetColor(255, 255, 50, op);
            font_menu.drawString("how to play", ofGetWidth() / 2 - 299 / 2, ofGetHeight() / 2 + 80);
        }

        break;
    }
    case PLAYING:
    {
        ofClear(0);

        // ofMesh mesh = make_mesh(currentImg);
        // ofPushMatrix();
        // ofTranslate(448,28);
        // mesh.draw();
        // ofPopMatrix();

        draw_distance_indicator(distance);

        //
        controller.draw();

        // draw main game surface
        ofEnableAlphaBlending();
        surface_main.draw();
        ofDisableAlphaBlending();

        // show zooming images
        ofEnableAlphaBlending();
        for (int i = warps.size() - 1; i >= 0; i--)
        {
            // cout << "drawing warp" << endl;
            warps[i].draw();
        }
        ofDisableAlphaBlending();

        // draw target location image
        if (targetImg.isAllocated())
        {
            // targetImg.draw(img_dims, 0);

            // draw to fbo instead and integrate into HUD
            targetFbo.begin();
            ofSetColor(255);
            targetImg.draw(0, 0);
            targetFbo.end();

            // draw the target image, allow toggle for larger view
            if (targetimg_maximized)
            {
                targetimg_x = ofLerp(targetimg_x, tiX_max, .05);
                targetimg_y = ofLerp(targetimg_y, tiY_max, .05);
                targetimg_dim = ofLerp(targetimg_dim, tiD_max, .05);
                ofSetColor(255);
                targetFbo.draw(targetimg_x, targetimg_y, targetimg_dim, targetimg_dim);

                float op = targetimg_dim / tiD_max * 255;
                draw_centered_text("use the controls\nto pilot your ship here", ofColor(255, 255, 255, op), 0);
            }
            else
            {

                targetimg_x = ofLerp(targetimg_x, tiX_min, .05);
                targetimg_y = ofLerp(targetimg_y, tiY_min, .05);
                targetimg_dim = ofLerp(targetimg_dim, tiD_min, .05);
                ofSetColor(255);
                targetFbo.draw(targetimg_x, targetimg_y, targetimg_dim, targetimg_dim);

                ofDrawBitmapString("TARGET DESINATION:", targetimg_x, targetimg_y - 10);
            }
        }

        if (game_startfade)
        {
            // fade out over time
            float op = ofMap(ofGetElapsedTimeMillis(), game_fadestartedtime,
                             game_fadestartedtime + game_fadetime, 512, 0);

            // mapping from 512 is a quick hack to make it completely opaque for the first half
            ofSetColor(0, op);
            ofFill();
            ofEnableAlphaBlending();
            ofRect(0, 0, ofGetWidth(), ofGetHeight());
            ofDisableAlphaBlending();
        }

        break;
    }
    case END:
    {

        // draw the end screen
        float op = 255;
        ofClear(0);

        surface_main.draw();

        // draw the menu
        if (end_startfade)
        {
            // fade out over time
            op = ofMap(ofGetElapsedTimeMillis(), end_fadestartedtime,
                       end_fadestartedtime + end_fadetime, 0, 512);
            // 512 is a trick to fade in faster then stay fully opaque
        }
        op = ofClamp(op, 0, 255);

        draw_centered_text("YOU WIN!", ofColor(0, op), 4);
        draw_centered_text("YOU WIN!", ofColor(255, 255, 0, op), 0);
        break;
    }

    case HELP_SCREEN:
    {

        // play tutorial vid
        ofSetColor(255, 255);
        tutorial_vid.draw(0, 0);

        // controls
        // ofDrawBitmapString("Here are your ship's controls. \nYou must ...:", 20, 348);
        // ofSetColor(255, 255, 0);
        // ofRect(20, 162, 408, 700);

        // // main surface
        // ofDrawBitmapString("This shows where you are in hyperdimensional space ...", 448, 28);
        // ofSetColor(0, 255, 255);
        // ofRect(448, 28, 1024, 1024);

        // // top right thing
        // ofDrawBitmapString("This is where I, the ship's computer, will \ngive you valuable advice on your journey", 1492, 88);
        // ofSetColor(255, 0, 255);
        // ofRect(1492, 88, 408, 408);

        // // target img
        // ofDrawBitmapString("This is the place to which you must navigate", 1492, 584 - 33);
        // ofSetColor(22, 255, 66);
        // ofRect(1492, 584, 408, 408);

        break;
    }
    }

    // show fps
    // string str = ofToString(ofGetFrameRate());
    // ofSetColor(255);
    // ofDrawBitmapString(str, 10, 10);

    // save frames for video
    if (save_frames)
    {
        ofSaveScreen("frames/" + ofToString(ofGetFrameNum()) + ".png");
    }
}
//--------------------------------------------------------------
void ofApp::draw_distance_indicator(float &distance)
{
    // was 1492, 88, 408, 408
    ofRectangle r(1492, 310, 408, 408);

    // do this instead with text for style?
    ofDrawBitmapString("PROGRESS TO TARGET:", 1492, r.y - 20);
    string background_distance = "|||||||||||||||||||||||||||||||||||||||||||||||||||";
    ofSetColor(88, 88, 0);
    ofDrawBitmapString(background_distance, 1492, r.y);

    string distance_string = "";

    // not sure if it makes sense to measure from the starting distance or
    // the farthest possible distance so why not compromise
    // float avg_max_dst = (starting_distance + max_dist) / 2;

    int mapped_d = ofMap(distance, min_distance, starting_distance, background_distance.length(), 0);
    mapped_d = ofClamp(mapped_d, 0, background_distance.length());

    for (int i = 0; i < mapped_d; i++)
    {
        distance_string += "|";
    }

    ofSetColor(255, 255, 0);
    ofDrawBitmapString(distance_string, 1492, r.y);
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
    controller.reset(starting_position, target_position, shuffled_vecs); // make sure controller starts at correct pos
    current_position = starting_position;

    starting_distance = find_distance(target_position, current_position);

    controls_changed = true;
    generate_image(current_position, truncation, CURRENT_IMAGE);

    // generate a new target based on which input vectors should be frozen
    // target_position = generate_new_target(starting_position, isolate_vectors);

    generate_image(target_position, truncation, TARGET_IMAGE);

    // reset other game vars
    lerp_amount = 0;

    // delete all warp meshes
    // warps.clear();
    // warp_effect(currentImg.getTexture(), origin);
}
//--------------------------------------------------------------
void ofApp::receive_button(string &button)
{

    if (GAME_STATE == MENU)
    {
        if (button == "up" || button == "left")
        {
            sound.play_once("menu_change");
            menu_selection--;
            if (menu_selection < 0)
                menu_selection = menu_count;
        }

        if (button == "down" || button == "right")
        {
            if (GAME_STATE == MENU)
            {
                sound.play_once("menu_change");
                menu_selection++;
                if (menu_selection > menu_count)
                    menu_selection = 0;
            }
        }
    }

    if (button == "fire")
    {
        if (GAME_STATE == MENU)
        {
            sound.play_once("menu_select");
            if (menu_selection == 0)
            {
                // GAME_STATE = PLAYING;
                menu_fadestartedtime = ofGetElapsedTimeMillis();
                menu_startfade = true;
            }

            if (menu_selection == 1)
            {
                GAME_STATE = HELP_SCREEN;
                tutorial_vid.setPosition(0);
                tutorial_vid.firstFrame();
                tutorial_vid.play();
            }
        }
        else if (GAME_STATE == HELP_SCREEN)
        {
            sound.play_once("menu_select");
            GAME_STATE = MENU;
        }
    }
}
//--------------------------------------------------------------
void ofApp::receive_control_vectors(vector<float> &controls)
{

    switch (GAME_STATE)
    {
    case MENU:

        break;

    case PLAYING:
        int index = 0;
        for (int i = 0; i < controls.size(); i++)
        {
            // controls is a series of 16 floats
            for (int v = 0; v < vecs_per_group; v++)
            {
                // set current position but based on the shuffled associated vectors
                current_position[shuffled_vecs[index]] = controls[i];

                index++;
            }
        }
        controls_changed = true;

        // make some noise
        sound.play_engine(controller.active_vec, controls);

        break;
    }
}
//--------------------------------------------------------------
void ofApp::change_active_vec(int &vec)
{
    if (GAME_STATE == PLAYING)
    {
        sound.change_active_vec(vec);
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    // menu
    switch (GAME_STATE)
    {
    case MENU:
        break;
    case PLAYING:
    {

        if (key == 'n')
        {
            // reset game
            newPosition();
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

    if (key == ' ')
    {
    }
    if (key == 'x')
    {
        current_position = target_position;
    }

    if (key == 'c')
    {
        lerp_ship();
    }
}
//--------------------------------------------------------------
void ofApp::lerp_ship()
{
    controller.lerp_ship();
    controls_changed = true;
}
//--------------------------------------------------------------
void ofApp::control_changed(ofAbstractParameter &e)
{

    // update_position();
    controls_changed = true;
}
//--------------------------------------------------------------
void ofApp::warp_effect(ofTexture &texture, ofVec3f location)
{
    // image warp
    ImageWarp iw = ImageWarp(texture, location);
    iw.fly = true;
    warps.push_back(iw);

    // find the oldest warp that isn't yet flying
    // warps[warps.size() - 2].fly = true;
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
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson &info)
{
    ofLogNotice("ofApp::runwayInfoEvent") << info.dump(2);
}
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string &message)
{
    ofLogNotice("ofApp::runwayErrorEvent") << message;
}
//--------------------------------------------------------------
void ofApp::draw_centered_text(string str, ofColor color, float offset)
{
    ofSetColor(color);
    font_menu.drawString(str, ofGetWidth() / 2 - font_menu.stringWidth(str) / 2 + offset, ofGetHeight() / 2 - font_menu.stringHeight(str) / 2 + offset);
}

// OLD CODE

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

// MIDI
// for (unsigned int i = 0; i < midiMessages.size(); ++i)
// {
//     ofxMidiMessage &message = midiMessages[i];

//     int c = message.control - 1;

//     if (c < num_isolated)
//     {
//         vecs.at(c) = ofMap(message.value, 0, 127, -min_max_vecs, min_max_vecs);
//         update_position();
//     }
// }

// // testing
// // controls
// ofSetColor(255, 255, 0);
// ofRect(20, 162, 408, 700);
// // main surface
// ofSetColor(0, 255, 255);
// ofRect(448, 28, 1024, 1024);
// // top right thing
// ofSetColor(255, 0, 255);
// ofRect(1492, 88, 408, 408);
// // target img
// ofSetColor(22, 255, 66);
// ofRect(1492, 584, 408, 408);

// plain text distance number
// ofRectangle r(ofGetWidth() / 2 - 50, ofGetHeight() - 30, 100, 20);

// ofSetColor(80);
// ofDrawRectangle(r);

// distance as text
// string diststr = ofToString(distance);
// ofSetColor(255);
// ofDrawBitmapString(diststr, r.getBottomLeft() + glm::vec3(0, 2, 0));

// background for meter
// ofRectangle distance_bg(10, ofGetHeight() - 40, ofGetWidth() - 20, 20);
// ofRectangle distance_bg(r.x + 10, r.y - 20, r.width - 20, 20);

// float mapped_dist = ofMap(distance, max_dist, 0, distance_bg.x, distance_bg.x + distance_bg.width);

// ofRectangle distance_meter(distance_bg.x, distance_bg.y, mapped_dist, distance_bg.height);
// ofSetColor(0);
// ofDrawRectangle(distance_bg);
// ofSetColor(255);
// ofDrawRectangle(distance_meter);

// ofVec3f arrowTailPoint(r.x + 10, r.y - 40, 0);
// ofVec3f arrowHeadPoint(mapped_dist, r.y - 40, 0);
// ofDrawArrow(arrowTailPoint, arrowHeadPoint, 4.0);

// ofSetColor(255);
// ofDrawBitmapString(ofToString(controller.active_vec), r.x + 20, r.y - 20);

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
// update_position();

// starting_position should take into account the current controller values
// starting_position = current_position;

// // setup gui
//     if (USE_GUI)
//     {
//         sliderGroup.setName("sliders");
//         for (size_t i{0}; i < num_isolated; ++i)
//         {
//             ofParameter<float> p;
//             std::string name = "vec ";

//             name += to_string(i);

//             // make sure to randomize vecs because if they are set to 0 it gets funky
//             float rand = ofRandom(-min_max_vecs, min_max_vecs);

//             p.set(name, rand, -min_max_vecs, min_max_vecs);
//             // p.addListener(this, &ofApp::control_changed);

//             vecs.push_back(p);
//             sliderGroup.add(vecs.at(i));
//         }
//         // add listener for the controls so we only update when necessary
//         ofAddListener(sliderGroup.parameterChangedE(), this, &ofApp::control_changed);
//         gui.setup(sliderGroup);
//     }