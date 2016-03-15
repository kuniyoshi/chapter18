#include "TheCollision.h"
#include <utility>
#include <vector>
#include "GraphicsDatabase/Vector3.h"
#include "Bullet.h"
#include "Cuboid.h"
#include "Robo.h"
#include "Segment.h"
#include "Sphere.h"
#include "TheHorizon.h"
#include "Triangle.h"
#include "Wall.h"

using GraphicsDatabase::Vector3;

void TheCollision::burn(    Bullet* bullet,
                            Robo* robo,
                            const Cuboid& cuboid,
                            const std::vector< Triangle >& triangles)
{
    Cuboid bullet_cuboid = bullet->locus_cuboid();

    if (!bullet_cuboid.does_intersect(cuboid))
    {
        return;
    }

    Segment segment = bullet->locus_segment();

    for (size_t i = 0; i < triangles.size(); ++i)
    {
        std::pair< bool, Vector3 > cp
        = segment.get_intersected_point(triangles.at(i));

        if (cp.first)
        {
            bullet->burn_at(cp.second);
            robo->was_shot(0.01);
        }
    }
}

void TheCollision::burn(Bullet* bullet, TheHorizon horizon)
{
    Segment segment = bullet->locus_segment();
    const std::vector< Triangle >* triangles = horizon.triangles();

    for (size_t i = 0; i < triangles->size(); ++i)
    {
        std::pair< bool, Vector3 > cp
        = segment.get_intersected_point(triangles->at(i));

        if (cp.first)
        {
            bullet->burn_at(cp.second);
        }
    }
}

void TheCollision::slide_next_move_if_collision_will_occur(Robo* robo)
{
    by_segment(robo);
}

void TheCollision::slide_next_move_if_collision_will_occur( Robo* robo,
                                                            const Robo* opponent)
{
    by_sphere(robo, opponent);
}

void TheCollision::slide_next_move_if_collision_will_occur( Robo* robo,
                                                            const Wall* wall)
{
    by_segment(robo, wall);
}

void TheCollision::by_cuboid(Robo* robo)
{
    Vector3 delta_next_position;
    Vector3 velocity;

    // TODO: skip for now, it make codes more long.
    // patterns:
    //   - x bad
    //   - y bad
    //   - z bad
    //   - xy bad
    //   - xz bad
    //   - yz bad
    //   - all bad

    delta_next_position = *(robo->delta_next_position());
    velocity = *(robo->velocity());

    if (robo->cuboid().does_intersect(TheHorizon::instance().cuboid()))
    {
        delta_next_position.y = 0.0;
        velocity.y = 0.0;
        robo->delta_next_position(delta_next_position);
        robo->velocity(velocity);
    }

    delta_next_position = *(robo->delta_next_position());
    velocity = *(robo->velocity());

    if (robo->cuboid().does_intersect(TheHorizon::instance().cuboid()))
    {
        delta_next_position.z = 0.0;
        velocity.z = 0.0;
        robo->delta_next_position(delta_next_position);
        robo->velocity(velocity);
    }

    delta_next_position = *(robo->delta_next_position());
    velocity = *(robo->velocity());

    if (robo->cuboid().does_intersect(TheHorizon::instance().cuboid()))
    {
        delta_next_position.x = 0.0;
        velocity.x = 0.0;
        robo->delta_next_position(delta_next_position);
        robo->velocity(velocity);
    }
}

void TheCollision::by_cuboid(Robo* robo, const Robo* opponent)
{
    Vector3 delta_next_position;
    Vector3 velocity;

    delta_next_position = *(robo->delta_next_position());
    velocity = *(robo->velocity());

    if (robo->cuboid().does_intersect(opponent->cuboid()))
    {
        delta_next_position.z = 0.0;
        velocity.z = 0.0;
        robo->delta_next_position(delta_next_position);
        robo->velocity(velocity);
    }

    delta_next_position = *(robo->delta_next_position());
    velocity = *(robo->velocity());

    if (robo->cuboid().does_intersect(opponent->cuboid()))
    {
        delta_next_position.x = 0.0;
        velocity.x = 0.0;
        robo->delta_next_position(delta_next_position);
        robo->velocity(velocity);
    }

    delta_next_position = *(robo->delta_next_position());
    velocity = *(robo->velocity());

    if (robo->cuboid().does_intersect(opponent->cuboid()))
    {
        delta_next_position.y = 0.0;
        velocity.y = 0.0;
        robo->delta_next_position(delta_next_position);
        robo->velocity(velocity);
    }
}

void TheCollision::by_sphere(Robo* robo)
{
    Sphere rs(robo->sphere()); // robo sphere
    Sphere hs(TheHorizon::instance().sphere()); // the horizon sphere

    if (!rs.does_intersect(hs))
    {
        return;
    }

    Vector3 d(*(robo->delta_next_position())); // delta

    Vector3 to_the_horizon(*(hs.balance()));
    to_the_horizon.subtract(*(rs.balance()));
    double length = to_the_horizon.length();
    to_the_horizon.multiply(to_the_horizon.dot(d));
    to_the_horizon.divide(length * length);

    d.subtract(to_the_horizon);

    robo->delta_next_position(d);

    Vector3 v(*(robo->velocity()));
    to_the_horizon = *(hs.balance());
    to_the_horizon.subtract(*(rs.balance()));
    length = to_the_horizon.length();
    to_the_horizon.multiply(to_the_horizon.dot(v));
    to_the_horizon.divide(length * length);

    v.subtract(to_the_horizon);

    robo->velocity(v);
}

namespace
{

bool does_theta_greater_than_90(const Vector3& a, const Vector3& b)
{
    Vector3 normalized_a(a);
    normalized_a.normalize(1.0);
    Vector3 normalized_b(b);
    normalized_b.normalize(1.0);
    normalized_a.add(normalized_b);
    return normalized_a.squared_length() < 2.0;
}

} // namespace -

void TheCollision::by_sphere(Robo* robo, const Robo* opponent)
{
    Sphere rs(robo->sphere()); // robo sphere
    Sphere os(opponent->sphere()); // opponent sphere

    if (!rs.does_intersect(os))
    {
        return;
    }

    Vector3 d(*(robo->delta_next_position())); // delta

    Vector3 to_the_opponent(*(os.balance()));
    to_the_opponent.subtract(*(rs.balance()));

    if (does_theta_greater_than_90(d, to_the_opponent))
    {
        return;
    }

    double length = to_the_opponent.length();
    to_the_opponent.multiply(to_the_opponent.dot(d));
    to_the_opponent.divide(length * length);

    d.subtract(to_the_opponent);

    robo->delta_next_position(d);

    Vector3 v(*(robo->velocity()));
    to_the_opponent = *(os.balance());
    to_the_opponent.subtract(*(rs.balance()));
    length = to_the_opponent.length();
    to_the_opponent.multiply(to_the_opponent.dot(v));
    to_the_opponent.divide(length * length);

    v.subtract(to_the_opponent);

    robo->velocity(v);
}

namespace
{

void robo_to_triangles(Robo* robo, const std::vector< Triangle >* triangles)
{
    const std::vector< Segment > segments = robo->segments();
    std::vector< Segment >::const_iterator it = segments.begin();
    std::pair< bool, Vector3 > collision_point(false, Vector3(0.0, 0.0, 0.0));

    for (size_t i = 0; i < triangles->size(); ++i)
    {
        for (it = segments.begin(); it != segments.end(); ++it)
        {
            collision_point = (*it).get_intersected_point(triangles->at(i));

            if (collision_point.first)
            {
                break;
            }
        }

        if (collision_point.first)
        {
            break;
        }
    }

    if (!collision_point.first)
    {
        return;
    }

    const Vector3* balance = robo->center();
    Vector3 to_collision_point(collision_point.second);
    to_collision_point.subtract(*balance);

    const Vector3* force = robo->force();
    double cosine = force->dot(to_collision_point)
    / (force->length() * to_collision_point.length());
    Vector3 delta(*force);
    delta.multiply(cosine);

    Vector3 new_value(*force);
    new_value.subtract(delta);
    robo->force(new_value);

    const Vector3* velocity = robo->velocity();
    cosine = velocity->dot(to_collision_point)
    / (velocity->length() * to_collision_point.length());
    delta = *velocity;
    delta.multiply(cosine);

    new_value = *velocity;
    new_value.subtract(delta);
    robo->velocity(new_value);

    const Vector3* delta_next_position = robo->delta_next_position();
    cosine = delta_next_position->dot(to_collision_point)
    / (delta_next_position->length() * to_collision_point.length());
    delta = *delta_next_position;
    delta.multiply(cosine);

    new_value = *delta_next_position;
    new_value.subtract(delta);
    robo->delta_next_position(new_value);
}

} // namespace -

void TheCollision::by_segment(Robo* robo)
{
    const std::vector< Triangle >* triangles = TheHorizon::instance().triangles();
    robo_to_triangles(robo, triangles);
}

void TheCollision::by_segment(Robo* robo, const Wall* wall)
{
    const std::vector< Triangle >* triangles = wall->triangles();
    robo_to_triangles(robo, triangles);
}
