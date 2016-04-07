#ifndef ROBOFTHEENVIRONMENT_H_
#define ROBOFTHEENVIRONMENT_H_

namespace GraphicsDatabase { class Vector3; }

using GraphicsDatabase::Vector3;

class TheEnvironment
{
public:
    static const unsigned MaxBattleMs;
    static double FollowRate;
    static const double GravityAcceleration;
    static unsigned RemainedBattleMs;
    static const double AmbientBrightness;
    static const Vector3 Brightness;
    static const Vector3 LightVector;

    static double gravity_acceleration();
    static void tick();
};

#endif
