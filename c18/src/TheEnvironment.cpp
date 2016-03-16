#include "TheEnvironment.h"
#include "GraphicsDatabase/Vector3.h"
#include "TheTime.h"

const unsigned TheEnvironment::MaxBattleMs          = 99000;
double TheEnvironment::FollowRate                   = 0.2;
const double TheEnvironment::GravityAcceleration    = 9.8;
unsigned TheEnvironment::RemainedBattleMs           = MaxBattleMs;
const double TheEnvironment::AmbientBrightness      = 0.2;
const Vector3 TheEnvironment::Brightness(   1.0 - AmbientBrightness,
                                            1.0 - AmbientBrightness,
                                            1.0 - AmbientBrightness);
const Vector3 TheEnvironment::LightVector(0.0, 1.0, 0.0);

double TheEnvironment::gravity_acceleration()
{
    return GravityAcceleration * TheTime::instance().rate();
}

void TheEnvironment::tick()
{
    unsigned dt = TheTime::instance().delta();

    // if (RemainedBattleMs > dt)
    // {
        RemainedBattleMs = RemainedBattleMs - dt;
    // }
    // else
    // {
    //     RemainedBattleMs = 0;
    // }
}
