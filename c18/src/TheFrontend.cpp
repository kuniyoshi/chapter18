#include "TheFrontend.h"
#include <cassert>
#include <utility>
#include "GameLib/Framework.h"
#include "TheEnvironment.h"

using std::pair;

namespace
{

void draw_rect( pair< double, double > p0,
                pair< double, double > p1,
                pair< double, double > p2,
                pair< double, double > p3,
                unsigned c0,
                unsigned c1,
                unsigned c2,
                unsigned c3,
                GameLib::Framework f)
{
    double q0[4];
    double q1[4];
    double q2[4];
    double q3[4];

    q0[0] = p0.first;
    q0[1] = p0.second;
    q0[2] = 0.2;
    q0[3] = 1.0;
    q1[0] = p1.first;
    q1[1] = p1.second;
    q1[2] = 0.2;
    q1[3] = 1.0;
    q2[0] = p2.first;
    q2[1] = p2.second;
    q2[2] = 0.2;
    q2[3] = 1.0;
    q3[0] = p3.first;
    q3[1] = p3.second;
    q3[2] = 0.2;
    q3[3] = 1.0;

    double uv0[2] = {0.0, 0.0};

    f.drawTriangle3DH(  q0, q1, q2,
                        uv0, uv0, uv0,
                        c0, c1, c3);
    f.drawTriangle3DH(  q1, q3, q2,
                        uv0, uv0, uv0,
                        c1, c3, c2);
}

} // namespace -

void TheFrontend::draw()
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.setBlendMode(GameLib::Framework::BLEND_LINEAR);
    f.enableDepthTest(true);
    f.enableDepthWrite(false);

    const double time_bar_left  = -0.8;
    const double time_bar_right = +0.8;
    const double time_bar_rate
    = static_cast< double >(TheEnvironment::RemainedBattleMs)
    / TheEnvironment::MaxBattleMs;
    assert(time_bar_rate <= 1.0 && time_bar_rate >= 0.0);
    const double current_time_bar
    = (time_bar_right - time_bar_left) * time_bar_rate + time_bar_left;

    draw_rect(  pair< double, double >(time_bar_left, 0.9),
                pair< double, double >(current_time_bar, 0.9),
                pair< double, double >(time_bar_left, 0.8),
                pair< double, double >(current_time_bar, 0.8),
                0xea055c9f,
                0xea055c9f,
                0xea055c9f,
                0xea055c9f,
                f);
}
