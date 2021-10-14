#include "sound.h"

Sound::Sound()
{
}

void Sound::setup()
{
    engine.setLoop(true);
    engine.setMultiPlay(true);
    
    menu_change.load("sounds/menu_change.wav");
    menu_select.load("sounds/menu_select.wav");
}

void Sound::update()
{
    pan += pan_speed;

    engine.setPan(sin(pan));
}

void Sound::play_once(string which_sound)
{
    if(which_sound == "menu_change")
    {
        menu_change.play();
    }

    if(which_sound == "menu_select")
    {
        menu_select.play();
    }
}

void Sound::play_engine(int active_vec, vector<float> &controls)
{
    engine.load("sounds/beam1.wav");
    engine.play();
    
}

void Sound::draw()
{
}
