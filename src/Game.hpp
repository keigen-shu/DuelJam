#ifndef GAME_H
#define GAME_H

#include <random>
#include <ClanLib/display.h>
#include <ClanLib/ui.h>
#include "clanExt_JSONFile.hpp"
#include "clanAlt_Font.hpp"
#include "AudioManager.hpp"
#include "InputManager.hpp"

/** Main game object */
class Game : public clan::WindowView
{
public:
    static JSONFile conf;
    static JSONFile skin;

    clan::DisplayWindow     clDW;
    clan::GraphicContext    clGC;
    clan::Canvas            clCv;

    AudioManager            am;
    InputManager            im;
    SimpleFont              font;

    bool                    keep_alive;

    Game(clan::DisplayWindowDescription &);

private:
    ////    CALLBACK TARGETS    ////
    void on_close(clan::CloseEvent &);
    void on_key_press(clan::KeyEvent &);

public:
    static std::shared_ptr<Game> create();
};

#endif
