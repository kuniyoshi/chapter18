#include "TheCollision.h"
#include "GraphicsDatabase/Vector3.h"
#include "Cuboid.h"
#include "Robo.h"
#include "Sphere.h"
#include "TheHorizon.h"

using GraphicsDatabase::Vector3;

void TheCollision::slide_next_move_if_collision_will_occur(Robo* robo)
{
    by_cuboid(robo);
}

void TheCollision::slide_next_move_if_collision_will_occur( Robo* robo,
                                                            Robo* opponent)
{
    by_cuboid(robo, opponent);
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
