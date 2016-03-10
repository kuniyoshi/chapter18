#ifndef ROBOF__THE_ENVIRONMENT_H_
#define ROBOF__THE_ENVIRONMENT_H_

class TheEnvironment
{
public:
    static const unsigned MaxBattleMs;
    static double FollowRate;
    static const double GravityAcceleration;
    static unsigned RemainedBattleMs;

    static double gravity_acceleration();
    static void tick();
};

#endif
