#include "sound.h"

Sound::Sound()
{
}

void Sound::setup()
{    
    menu_music.load("sounds/menu_music.wav");
    menu_music.setLoop(true);
    menu_change.load("sounds/vector_change/vector_change_2.wav");
    menu_select.load("sounds/hot_sounds/hot01.wav");

    engine.setLoop(true);
    engine.setMultiPlay(true);
    game_music.load("sounds/background_music.wav");
    game_music.setLoop(true);

    // this probably shouldn't be hard coded...
    cout << "loading in sounds ...";
    for (int i = 0; i < 16; i++)
    {
        ofSoundPlayer osp;
        string fn = "data/sounds/vector_change/vector_change_" + ofToString(i) + ".wav";
        osp.load(fn);
        vec_change.push_back(osp);
        cout << fn;
    }
    cout << endl;

    victory.load("sounds/victory_sound.wav");
}

void Sound::update()
{
    pan += pan_speed;

    engine.setPan(sin(pan));

    // fade in game music
    if (game_music_fading_in)
    {
        float vol = ofMap(ofGetElapsedTimeMillis(), game_music_started_fade, game_music_started_fade + game_music_fade_duration, 0, 1);
        game_music.setVolume(vol);

        if (vol >= 0.99)
        {
            vol = 1.0;
            game_music_fading_in = false;
        }
    }

    // fade out game music
    if (game_music_fading_out)
    {
        float vol = ofMap(ofGetElapsedTimeMillis(), game_music_started_fade, game_music_started_fade + game_music_fade_duration, 1, 0);
        game_music.setVolume(vol);

        if (vol <= 0.01)
        {
            vol = 0.0;
            game_music_fading_out = false;
        }
    }

    // fade in menu music
    if (menu_music_fading_in)
    {
        float vol = ofMap(ofGetElapsedTimeMillis(), menu_music_started_fade, menu_music_started_fade + menu_music_fade_duration, 0, 1);
        menu_music.setVolume(vol);

        if (vol >= 0.99)
        {
            vol = 1.0;
            menu_music_fading_in = false;
        }
    }

    // fade out menu music
    if (menu_music_fading_out)
    {
        float vol = ofMap(ofGetElapsedTimeMillis(), menu_music_started_fade, menu_music_started_fade + menu_music_fade_duration, 1, 0);
        menu_music.setVolume(vol);

        if (vol <= 0.01)
        {
            vol = 0.0;
            menu_music_fading_out = false;
        }
    }
}

void Sound::menu_start()
{
    menu_music.play();
    menu_music_fading_in = true;
    menu_music_started_fade = ofGetElapsedTimeMillis();
}

void Sound::menu_end()
{
    menu_music_fading_out = true;
    menu_music_started_fade = ofGetElapsedTimeMillis();
}

void Sound::game_start()
{
    game_music.play();
    game_music_fading_in = true;
    game_music_started_fade = ofGetElapsedTimeMillis();
}

void Sound::game_end()
{
    game_music_fading_out = true;
    game_music_started_fade = ofGetElapsedTimeMillis();
}

void Sound::play_once(string which_sound)
{
    if (which_sound == "menu_change")
    {
        menu_change.play();
    }

    if (which_sound == "menu_select")
    {
        menu_select.play();
    }

    if (which_sound == "victory")
    {
        victory.play();
    }

    // receive a vector change signal...
    if (which_sound.find("vec_change") != std::string::npos)
    {
        // ... get the #
        // For atoi, the input string has to start with a digit, so lets search for the first digit
        size_t i = 0;
        for (; i < which_sound.length(); i++)
        {
            if (isdigit(which_sound[i]))
                break;
        }

        // remove the first chars, which aren't digits
        which_sound = which_sound.substr(i, which_sound.length() - i);

        // convert the remaining text to an integer
        int id = atoi(which_sound.c_str());

        cout << which_sound << " detected" << endl;

        vec_change[id].play();
    }
}

void Sound::play_engine(int active_vec, vector<float> &controls)
{
    // engine.load("sounds/beam1.wav");
    // engine.play();
}

void Sound::draw()
{
}

void Sound::change_active_vec(int &v)
{
    play_once("vec_change" + ofToString(v));
}
