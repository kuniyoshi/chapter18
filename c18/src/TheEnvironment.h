#ifndef ROBOF__THE_ENVIRONMENT_H_
#define ROBOF__THE_ENVIRONMENT_H_

class TheEnvironment
{
public:
    static const unsigned MaxBattleMs;
    static double FollowRate;
    static unsigned RemainedBattleMs;

    static void tick();
};

#endif
