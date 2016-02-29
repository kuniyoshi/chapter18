#include "Pad.h"
#include <map>
#include <utility>
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Manager.h"

namespace
{

char key_at(Pad::Input input)
{
    char key = '\0';

    switch (input)
    {
        case Pad::LeftStickUp:      key = 'w'; break;
        case Pad::LeftStickDown:    key = 'z'; break;
        case Pad::LeftStickLeft:    key = 'a'; break;
        case Pad::LeftStickRight:   key = 's'; break;
        case Pad::LeftTrigger:      key = 'x'; break;
        case Pad::RightTrigger:     key = 'c'; break;
        case Pad::RightStickUp:     key = 'k'; break;
        case Pad::RightStickDown:   key = 'j'; break;
        case Pad::RightStickLeft:   key = 'h'; break;
        case Pad::RightStickRight:  key = 'l'; break;
        case Pad::A:                key = 'n'; break;
        case Pad::B:                key = ' '; break;
        case Pad::Option:           key = 'y'; break;
        case Pad::Option2:          key = 'u'; break;
        case Pad::Reset:            key = 'r'; break;
        case Pad::Terminate:        key = 't'; break;
    }

    return key;
}

} // namespace

Pad::Pad(int id) : id_(id) {}

Pad::~Pad() {}

bool Pad::isOn(Input input) const
{
    GameLib::Input::Keyboard keyboard
    = GameLib::Input::Manager::instance().keyboard();

    return keyboard.isOn(key_at(input));
}

bool Pad::isTriggered(Input input) const
{
    GameLib::Input::Keyboard keyboard
    = GameLib::Input::Manager::instance().keyboard();

    return keyboard.isTriggered(key_at(input));
}
