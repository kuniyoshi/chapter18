#include "TheFrontend.h"
#include <cassert>
#include <cmath>
#include <utility>
#include "GameLib/Framework.h"
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Vector3.h"
#include "Robo.h"
#include "TheEnvironment.h"
#include "View.h"

using std::pair;
using GraphicsDatabase::Matrix44;
using GraphicsDatabase::Vector3;

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
                GameLib::Framework f,
                double z = 0.0,
                double w = 1.0)
{
    double q0[4];
    double q1[4];
    double q2[4];
    double q3[4];

    q0[0] = p0.first;
    q0[1] = p0.second;
    q0[2] = z;
    q0[3] = w;
    q1[0] = p1.first;
    q1[1] = p1.second;
    q1[2] = z;
    q1[3] = w;
    q2[0] = p2.first;
    q2[1] = p2.second;
    q2[2] = z;
    q2[3] = w;
    q3[0] = p3.first;
    q3[1] = p3.second;
    q3[2] = z;
    q3[3] = w;

    f.drawTriangle3DH(  q0, q1, q2,
                        0, 0, 0,
                        c0, c1, c3);
    f.drawTriangle3DH(  q1, q3, q2,
                        0, 0, 0,
                        c1, c3, c2);
}

unsigned calc_gradation_color(unsigned from, unsigned to, double rate)
{
    unsigned alpha      = (from & 0xff000000) >> 24;
    unsigned red_from   = (from & 0x00ff0000) >> 16;
    unsigned green_from = (from & 0x0000ff00) >> 8;
    unsigned blue_from  = (from & 0x000000ff) >> 0;
    unsigned red_to     = (to & 0x00ff0000) >> 16;
    unsigned green_to   = (to & 0x0000ff00) >> 8;
    unsigned blue_to    = (to & 0x000000ff) >> 0;

    unsigned red = (red_from > red_to)
    ? red_from - static_cast< unsigned >((red_from - red_to) * rate)
    : red_from + static_cast< unsigned >((red_to - red_from) * rate);
    unsigned green = (green_from > green_to)
    ? green_from - static_cast< unsigned >((green_from - green_to) * rate)
    : green_from + static_cast< unsigned >((green_to - green_from) * rate);
    unsigned blue = (blue_from > blue_to)
    ? blue_from - static_cast< unsigned >((blue_from - blue_to) * rate)
    : blue_from + static_cast< unsigned >((blue_to - blue_from) * rate);

    unsigned color = (alpha << 24);
    color = color + ((red & 0x000000ff) << 16);
    color = color + ((green & 0x000000ff) << 8);
    color = color + ((blue & 0x000000ff) << 0);

    return color;
}

unsigned calc_gradation_color_non_linear(   unsigned from,
                                            unsigned to,
                                            double rate)
{
    return calc_gradation_color(from, to, std::pow(rate, 2.0));
}

} // namespace -

void TheFrontend::draw(const Robo& player, const Robo& opponent)
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.setTexture(0);
    f.setBlendMode(GameLib::Framework::BLEND_LINEAR);
    f.enableDepthTest(true);
    f.enableDepthWrite(false);

    // time bar
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

    // energy
    const double energy_bar_top     = +0.8;
    const double energy_bar_bottom  = -0.8;
    const double energy_bar_rate    = player.energy();
    const double energy_bar_current
    = (energy_bar_top - energy_bar_bottom) * energy_bar_rate
    + energy_bar_bottom;

    draw_rect(  pair< double, double >(-0.9, energy_bar_current),
                pair< double, double >(-0.8, energy_bar_current),
                pair< double, double >(-0.9, energy_bar_bottom),
                pair< double, double >(-0.8, energy_bar_bottom),
                0xea2a561e,
                0xea2a561e,
                0xea2a561e,
                0xea2a561e,
                f);

    // hp
    const double hp_bar_left    = -0.8;
    const double hp_bar_right   = +0.8;
    const double hp = player.hp();
    const double current_hp = (hp_bar_right - hp_bar_left) * hp + hp_bar_left;

    draw_rect(  pair< double, double >(hp_bar_left, -0.8),
                pair< double, double >(current_hp, -0.8),
                pair< double, double >(hp_bar_left, -0.9),
                pair< double, double >(current_hp, -0.9),
                0xeab2a770,
                0xeab2a770,
                0xeab2a770,
                0xeab2a770,
                f);

    // opponent hp
    Matrix44 transformation(player.view()->get_perspective_matrix());
    Vector3 opponent_point(*opponent.center());
    transformation.multiply(&opponent_point);

    const double opponent_hp_bar_left   = opponent_point.x - 0.5;
    const double opponent_hp_bar_right  = opponent_point.x + 0.5;
    const double opponent_hp = opponent.hp();
    const double opponent_current_hp
    = (opponent_hp_bar_right - opponent_hp_bar_left) * opponent_hp
    + opponent_hp_bar_left;

    draw_rect(  pair< double, double >( opponent_hp_bar_left,
                                        opponent_point.y + 1.3),
                pair< double, double >( opponent_current_hp,
                                        opponent_point.y + 1.3),
                pair< double, double >( opponent_hp_bar_left,
                                        opponent_point.y + 1.1),
                pair< double, double >( opponent_current_hp,
                                        opponent_point.y + 1.1),
                0xeab2a770,
                0xeab2a770,
                0xeab2a770,
                0xeab2a770,
                f,
                0.0,
                opponent_point.w);
}
