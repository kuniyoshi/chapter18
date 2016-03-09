#include "TheFrontend.h"
#include <cassert>
#include <cmath>
#include <utility>
#include "GameLib/Framework.h"
#include "GameLib/Math.h"
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

void rotate(pair< double, double >* dot, double theta)
{
    double cosine = GameLib::cos(theta);
    double sine = GameLib::sin(theta);
    double x = dot->first;
    double y = dot->second;
    dot->first = cosine * x - sine * y;
    dot->second = cosine * x + sine * y;
}

void draw_horizontal_line(  pair< double, double > from,
                            pair< double, double > to,
                            double thickness,
                            unsigned from_color,
                            unsigned to_color,
                            GameLib::Framework f,
                            double z = 0.0,
                            double w = 1.0)
{
    double q0[4];
    double q1[4];
    double q2[4];
    double q3[4];

    q0[0] = from.first;
    q0[1] = from.second + thickness;
    q0[2] = z;
    q0[3] = w;
    q1[0] = to.first;
    q1[1] = to.second + thickness;
    q1[2] = z;
    q1[3] = w;
    q2[0] = from.first;
    q2[1] = from.second - thickness;
    q2[2] = z;
    q2[3] = w;
    q3[0] = to.first;
    q3[1] = to.second - thickness;
    q3[2] = z;
    q3[3] = w;

    f.drawTriangle3DH(  q0, q1, q2,
                        0, 0, 0,
                        from_color, to_color, from_color);
    f.drawTriangle3DH(  q1, q3, q2,
                        0, 0, 0,
                        to_color, to_color, from_color);
}

void draw_vertical_line(    pair< double, double > from,
                            pair< double, double > to,
                            double thickness,
                            unsigned from_color,
                            unsigned to_color,
                            GameLib::Framework f,
                            double z = 0.0,
                            double w = 1.0)
{
    double q0[4];
    double q1[4];
    double q2[4];
    double q3[4];

    q0[0] = from.first - thickness;
    q0[1] = from.second;
    q0[2] = z;
    q0[3] = w;
    q1[0] = from.first + thickness;
    q1[1] = from.second;
    q1[2] = z;
    q1[3] = w;
    q2[0] = to.first - thickness;
    q2[1] = to.second;
    q2[2] = z;
    q2[3] = w;
    q3[0] = to.first + thickness;
    q3[1] = to.second;
    q3[2] = z;
    q3[3] = w;

    f.drawTriangle3DH(  q0, q1, q2,
                        0, 0, 0,
                        from_color, from_color, to_color);
    f.drawTriangle3DH(  q1, q3, q2,
                        0, 0, 0,
                        from_color, to_color, to_color);
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

void draw_time_bar()
{
    GameLib::Framework f = GameLib::Framework::instance();

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

void draw_energy_bar(const Robo& player)
{
    GameLib::Framework f = GameLib::Framework::instance();

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
}

void draw_hp_bar(const Robo& player)
{
    GameLib::Framework f = GameLib::Framework::instance();

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
}

void draw_opponent_hp_bar(const Robo& player, const Robo& opponent)
{
    GameLib::Framework f = GameLib::Framework::instance();

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

void draw_lock_on_sight(const Robo& player, const Robo& opponent)
{
    GameLib::Framework f = GameLib::Framework::instance();

    const double size = player.get_half_sight_size_at_depth(opponent);
    const double depth = player.get_sight_depth(opponent);
    const double lock_on_rate = player.get_lock_on_rate();
    const unsigned to_color = 0xeaf36c2e;
    const unsigned from_color
    = calc_gradation_color_non_linear(0xeac3dc0c, to_color, lock_on_rate);

    draw_horizontal_line(   pair< double, double >(-size, +size),
                            pair< double, double >(+size, +size),
                            0.06,
                            from_color,
                            to_color,
                            f,
                            0.0,
                            depth);
    draw_vertical_line( pair< double, double >(+size, +size),
                        pair< double, double >(+size, -size),
                        0.06,
                        from_color,
                        to_color,
                        f,
                        0.0,
                        depth);
    draw_horizontal_line(   pair< double, double >(-size, -size),
                            pair< double, double >(+size, -size),
                            0.06,
                            from_color,
                            to_color,
                            f,
                            0.0,
                            depth);
    draw_vertical_line( pair< double, double >(-size, +size),
                        pair< double, double >(-size, -size),
                        0.06,
                        from_color,
                        to_color,
                        f,
                        0.0,
                        depth);
}

double my_scale(double x, double max_abs)
{
    if (x > 0.0)
    {
        if (x > max_abs)
        {
            return 1.0;
        }
        return x / max_abs;
    }
    else if (x < 0.0)
    {
        if (x < -max_abs)
        {
            return -1.0;
        }
        return x / max_abs;
    }
    return 0.0;
}

void draw_radar_map(const Robo& player, const Robo& opponent)
{
    Matrix44 transformation;
    transformation.translate(-*player.center());
    transformation.rotate_zx(player.view()->angle()->y);

    Vector3 opponent_point(*opponent.center());
    transformation.multiply(&opponent_point);

    const double opponent_y = opponent.center()->y;
    const double player_y = player.center()->y;
    const double max_abs_delta = 50.0;
    double delta_y = opponent_y - player_y;

    if (delta_y > max_abs_delta)
    {
        delta_y = max_abs_delta;
    }
    else if (delta_y < -max_abs_delta)
    {
        delta_y = -max_abs_delta;
    }

    const double delta_y_rate = (delta_y + max_abs_delta)
    / (2.0 * max_abs_delta);
    const unsigned color_from = 0xea140cdc;
    const unsigned color_to = 0xeadc0c0c;
    const unsigned opponent_color = calc_gradation_color(   color_from,
                                                            color_to,
                                                            delta_y_rate);
    const unsigned player_color = calc_gradation_color( color_from,
                                                        color_to,
                                                        0.5);

    opponent_point.y = 0.0;
    const double max_abs_length = 20.0;
    opponent_point.x = my_scale(opponent_point.x, max_abs_length);
    opponent_point.z = my_scale(opponent_point.z, max_abs_length);

    const pair< double, double > center(0.825, 0.675);
    const double half_size = 0.125;

    const pair< double, double > top_left(  center.first - half_size,
                                            center.second + half_size);
    const pair< double, double > top_right( center.first + half_size,
                                            center.second + half_size);
    const pair< double, double > bottom_left(   center.first - half_size,
                                                center.second - half_size);
    const pair< double, double > bottom_right(  center.first + half_size,
                                                center.second - half_size);

    GameLib::Framework f = GameLib::Framework::instance();
    const unsigned line_color = 0xea1a4404;

    draw_horizontal_line(   top_left,
                            top_right,
                            0.005,
                            line_color,
                            line_color,
                            f);
    draw_horizontal_line(   bottom_left,
                            bottom_right,
                            0.005,
                            line_color,
                            line_color,
                            f);
    draw_vertical_line( top_left,
                        bottom_left,
                        0.005,
                        line_color,
                        line_color,
                        f);
    draw_vertical_line( top_right,
                        bottom_right,
                        0.005,
                        line_color,
                        line_color,
                        f);

    const double player_half_size = 0.01;
    const double player_left = center.first - player_half_size;
    const double player_right = center.first + player_half_size;
    const double player_top = center.second + player_half_size;
    const double player_bottom = center.second - player_half_size;
    const pair< double, double > player_top_left(player_left, player_top);
    const pair< double, double > player_top_right(player_right, player_top);
    const pair< double, double > player_bottom_left(    player_left,
                                                        player_bottom);
    const pair< double, double > player_bottom_right(   player_right,
                                                        player_bottom);
    draw_rect(  player_top_left,
                player_top_right,
                player_bottom_left,
                player_bottom_right,
                player_color,
                player_color,
                player_color,
                player_color,
                f);

    const double opponent_half_size = 0.01;
    opponent_point.scale(half_size);
    const pair< double, double > opponent_center(   center.first
                                                    + opponent_point.x,
                                                    center.second
                                                    + -opponent_point.z);
    const double opponent_left = opponent_center.first - opponent_half_size;
    const double opponent_right = opponent_center.first + opponent_half_size;
    const double opponent_top = opponent_center.second + opponent_half_size;
    const double opponent_bottom = opponent_center.second - opponent_half_size;
    const pair< double, double > opponent_top_left( opponent_left,
                                                    opponent_top);
    const pair< double, double > opponent_top_right(    opponent_right,
                                                        opponent_top);
    const pair< double, double > opponent_bottom_left(  opponent_left,
                                                        opponent_bottom);
    const pair< double, double > opponent_bottom_right( opponent_right,
                                                        opponent_bottom);
    draw_rect(  opponent_top_left,
                opponent_top_right,
                opponent_bottom_left,
                opponent_bottom_right,
                opponent_color,
                opponent_color,
                opponent_color,
                opponent_color,
                f);
}

} // namespace -

void TheFrontend::draw(const Robo& player, const Robo& opponent)
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.setTexture(0);
    f.setBlendMode(GameLib::Framework::BLEND_LINEAR);
    f.enableDepthTest(true);
    f.enableDepthWrite(false);

    draw_time_bar();
    draw_energy_bar(player);
    draw_hp_bar(player);
    draw_opponent_hp_bar(player, opponent);
    draw_lock_on_sight(player, opponent);
    draw_radar_map(player, opponent);
}
