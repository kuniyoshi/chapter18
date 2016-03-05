#include "TheEnvironment.h"
#include "TheTime.h"

const unsigned TheEnvironment::MaxBattleMs = 99000;

double TheEnvironment::FollowRate = 0.2;
unsigned TheEnvironment::RemainedBattleMs = MaxBattleMs;

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