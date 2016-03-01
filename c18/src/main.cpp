#include "GameLib/Framework.h"
#include "GraphicsDatabase/Vector3.h"
#include "Ai/TheArmoury.h"
#include "Pad.h"
#include "Robo.h"
#include "TheCollision.h"
#include "TheDatabase.h"
#include "TheDebugOutput.h"
#include "TheHorizon.h"
#include "TheTime.h"
#include "View.h"
#include "Wall.h"

using namespace std;
using GraphicsDatabase::Vector3;

namespace GameLib
{

const double NearClip   = 0.5;
const double FarClip    = 1000.0;

View* g_view = 0;
Robo* g_robo = 0;
Robo* g_opponent = 0;
Wall* g_wall = 0;

void make_sure_globals_are()
{
    if (!TheTime::did_create())
    {
        TheTime::create();
    }

    if (!TheHorizon::did_create())
    {
        TheHorizon::create();
    }

    if (!TheDatabase::did_create())
    {
        TheDatabase::create();
    }

    if (!Ai::TheArmoury::did_create())
    {
        Ai::TheArmoury::create();
    }

    if (!g_view)
    {
        GameLib::Framework f = GameLib::Framework::instance();
        g_view = new View(f.width(), f.height(), NearClip, FarClip);
    }

    if (!g_robo)
    {
        g_robo = new Robo("myrobo");
        g_robo->warp(Vector3(0.0, 10.0, -1.0));
        g_robo->set_model_angle_zx(180.0);
    }

    if (!g_opponent)
    {
        g_opponent = new Robo("opponent");
        g_opponent->warp(Vector3(0.0, 10.0, -20));
    }

    if (!g_wall)
    {
        g_wall = new Wall("wall");
        g_wall->warp(Vector3(0.0, 1.2, -5.0));
    }
}

void clear_globals()
{
    TheTime::destroy();
    TheHorizon::destroy();
    Ai::TheArmoury::destroy();
    TheDatabase::destroy();
    SAFE_DELETE(g_view);
    SAFE_DELETE(g_robo);
    SAFE_DELETE(g_opponent);
    SAFE_DELETE(g_wall);
}

void Framework::update()
{
    make_sure_globals_are();

    TheTime::instance().tick();
    TheDebugOutput::clear();

    TheDebugOutput::print(frameRate());

    Vector3 move_direction;

    Pad pad(0);

    if (pad.isOn(Pad::LeftStickUp))
    {
        move_direction.add(Vector3(0.0, 0.0, +1.0));
    }
    if (pad.isOn(Pad::LeftTrigger))
    {
        move_direction.add(Vector3(+1.0, 0.0, 0.0));
    }
    if (pad.isOn(Pad::RightTrigger))
    {
        move_direction.add(Vector3(-1.0, 0.0, 0.0));
    }
    if (pad.isOn(Pad::LeftStickDown))
    {
        move_direction.add(Vector3(0.0, 0.0, -1.0));
    }

    if (move_direction.length() > 0)
    {
        move_direction.normalize(1.0);
        g_robo->run(move_direction);
    }

    if (pad.isOn(Pad::A))
    {
        g_robo->fire_bullet(g_view->angle());
    }

    if (pad.isOn(Pad::B))
    {
        g_robo->boost(move_direction);
    }

    if (pad.isOn(Pad::LeftStickRight))
    {
        g_robo->rotate_zx(-1);
    }

    if (pad.isOn(Pad::LeftStickLeft))
    {
        g_robo->rotate_zx(1);
    }

    g_view->follow(*g_robo);

    Vector3 angle_diff;

    if (pad.isOn(Pad::RightStickLeft))
    {
        angle_diff.add(Vector3(0.0, -1.0, 0.0));
    }
    if (pad.isOn(Pad::RightStickDown))
    {
        angle_diff.add(Vector3(-1.0, 0.0, 0.0));
    }
    if (pad.isOn(Pad::RightStickUp))
    {
        angle_diff.add(Vector3(+1.0, 0.0, 0.0));
    }
    if (pad.isOn(Pad::RightStickRight))
    {
        angle_diff.add(Vector3(0.0, +1.0, 0.0));
    }

    if (angle_diff.length() > 0)
    {
        g_view->rotate(angle_diff);
    }

    Ai::TheArmoury::instance().update();

    g_robo->update();
    g_opponent->update();
    TheCollision::slide_next_move_if_collision_will_occur(g_robo);
    TheCollision::slide_next_move_if_collision_will_occur(g_robo, g_opponent);
    TheCollision::slide_next_move_if_collision_will_occur(g_robo, g_wall);
    TheCollision::slide_next_move_if_collision_will_occur(g_opponent);
    TheCollision::slide_next_move_if_collision_will_occur(g_opponent, g_robo);
    TheCollision::slide_next_move_if_collision_will_occur(g_opponent, g_wall);
    g_robo->commit_next_position();
    g_opponent->commit_next_position();

    TheDebugOutput::print(*g_robo);
    TheDebugOutput::print(*g_view);

    g_robo->draw(*g_view);
    g_opponent->draw(*g_view);
    TheHorizon::instance().draw(*g_view);
    g_wall->draw(*g_view);
    Ai::TheArmoury::instance().draw(*g_view);

    if (pad.isOn(Pad::Reset))
    {
        clear_globals();
    }

    if (pad.isOn(Pad::Terminate))
    {
        requestEnd();
    }

    if (isEndRequested())
    {
        clear_globals();
    }
}

} // namespace GameLib
