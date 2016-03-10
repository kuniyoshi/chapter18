#include "TheEnvironment.h"
#include "TheTime.h"

const unsigned TheEnvironment::MaxBattleMs          = 99000;
double TheEnvironment::FollowRate                   = 0.2;
const double TheEnvironment::GravityAcceleration    = 9.8;
unsigned TheEnvironment::RemainedBattleMs           = MaxBattleMs;

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
