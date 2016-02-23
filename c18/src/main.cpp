#include <sstream>
#include "GameLib/Framework.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Input/Manager.h"
#include "GraphicsDatabase/Vector3.h"
#include "Robo.h"
#include "TheHorizon.h"
#include "TheTime.h"
#include "View.h"

using namespace std;
using GraphicsDatabase::Vector3;

namespace GameLib
{

const double NearClip   = 0.5;
const double FarClip    = 1000.0;

bool does_the_time_exist = false;
View* g_view = 0;
Robo* g_robo = 0;
Robo* g_opponent = 0;

void Framework::update()
{
    // make sure globals are
    {
        if (!does_the_time_exist)
        {
            TheTime::create();
            does_the_time_exist = true;
        }

        if (!TheHorizon::did_create())
        {
            TheHorizon::create();
        }

        if (!g_view)
        {
            g_view = new View(width(), height(), NearClip, FarClip);
        }

        if (!g_robo)
        {
            g_robo = new Robo();
            g_robo->rotate_zx(180 * 3);
        }

        if (!g_opponent)
        {
            g_opponent = new Robo();
            g_opponent->warp(Vector3(0.0, 0.0, -20));
        }
    }

    TheTime::instance().tick();

    enableDepthTest(true);
    enableDepthWrite(true);

    Vector3 move_direction;

    Input::Keyboard keyboard = Input::Manager::instance().keyboard();

    if (keyboard.isOn('W'))
    {
        move_direction.add(Vector3(0.0, 0.0, +1.0));
    }
    if (keyboard.isOn('X'))
    {
        move_direction.add(Vector3(+1.0, 0.0, 0.0));
    }
    if (keyboard.isOn('C'))
    {
        move_direction.add(Vector3(-1.0, 0.0, 0.0));
    }
    if (keyboard.isOn('Z'))
    {
        move_direction.add(Vector3(0.0, 0.0, -1.0));
    }

    if (move_direction.length() > 0)
    {
        move_direction.normalize(0.05);
        g_robo->run(move_direction);
    }

    if (keyboard.isOn(' '))
    {
        g_robo->boost(move_direction);
    }

    if (keyboard.isOn('S'))
    {
        g_robo->rotate_zx(-1);
    }

    if (keyboard.isOn('A'))
    {
        g_robo->rotate_zx(1);
    }

    g_view->follow(*g_robo);

    Vector3 angle_diff;

    if (keyboard.isOn('H'))
    {
        angle_diff.add(Vector3(+1.0, 0.0, 0.0));
    }
    if (keyboard.isOn('J'))
    {
        angle_diff.add(Vector3(0.0, -1.0, 0.0));
    }
    if (keyboard.isOn('K'))
    {
        angle_diff.add(Vector3(0.0, +1.0, 0.0));
    }
    if (keyboard.isOn('L'))
    {
        angle_diff.add(Vector3(-1.0, 0.0, 0.0));
    }

    if (angle_diff.length() > 0)
    {
        g_view->rotate(angle_diff);
    }

    if (keyboard.isOn('Y'))
    {
        g_view->increase_angle_of_view(1);
    }

    if (keyboard.isOn('U'))
    {
        g_view->decrease_angle_of_view(1);
    }

    g_robo->tick();

    g_robo->draw(*g_view);
    g_opponent->draw(*g_view);
    TheHorizon::instance().draw(*g_view);

    ostringstream oss;
    oss << frameRate();
    drawDebugString(0, 0, oss.str().c_str());
    oss.str("");

    if (keyboard.isTriggered('R'))
    {
        TheTime::destroy();
        does_the_time_exist = false;

        TheHorizon::destroy();
        SAFE_DELETE(g_view);
        SAFE_DELETE(g_robo);
        SAFE_DELETE(g_opponent);
    }

    if (Input::Manager::instance().keyboard().isTriggered('T'))
    {
        requestEnd();
    }

    if (isEndRequested())
    {
        TheTime::destroy();
        does_the_time_exist = false;
        TheHorizon::destroy();

        SAFE_DELETE(g_view);
        SAFE_DELETE(g_robo);
        SAFE_DELETE(g_opponent);
    }
}

} // namespace GameLib
