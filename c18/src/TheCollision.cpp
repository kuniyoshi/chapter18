#include "TheCollision.h"
#include <vector>
#include "GraphicsDatabase/Vector3.h"
#include "Cuboid.h"
#include "Robo.h"
#include "Segment.h"
#include "Sphere.h"
#include "TheHorizon.h"
#include "Triangle.h"

using GraphicsDatabase::Vector3;

void TheCollision::slide_next_move_if_collision_will_occur(Robo* robo)
{
    by_segment(robo);
}

void TheCollision::slide_next_move_if_collision_will_occur( Robo* robo,
                                                            Robo* opponent)
{
    by_sphere(robo, opponent);
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

void TheCollision::by_cuboid(Robo* robo, Robo* opponent)
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

void TheCollision::by_sphere(Robo* robo, Robo* opponent)
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

void TheCollision::by_segment(Robo* robo)
{
    std::vector< Triangle > triangles = TheHorizon::instance().triangles();
    const Segment segment = robo->segment();
    std::pair< bool, Vector3 > collision_point(false, Vector3(0.0, 0.0, 0.0));

    for (int i = 0; i < 2; ++i)
    {
        collision_point = segment.get_intersected_point(triangles[i]);

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
