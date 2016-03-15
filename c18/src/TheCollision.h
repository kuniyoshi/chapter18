#ifndef ROBOF__THE_COLLISION_H_
#define ROBOF__THE_COLLISION_H_
#include <vector>

class Bullet;
class Cuboid;
class Robo;
class TheHorizon;
class Triangle;
class Wall;

class TheCollision
{
public:
    static void burn(   Bullet* bullet,
                        Robo* robo,
                        const Cuboid& cuboid,
                        const std::vector< Triangle >& triangles);
    static void burn(Bullet* bullet, TheHorizon horizon);
    static void slide_next_move_if_collision_will_occur(Robo* robo);
    static void slide_next_move_if_collision_will_occur(    Robo* robo,
                                                            const Robo* opponent);
    static void slide_next_move_if_collision_will_occur(    Robo* robo,
                                                            const Wall* wall);

private:
    static void by_cuboid(Robo* robo);
    static void by_cuboid(Robo* robo, const Robo* opponent);
    static void by_sphere(Robo* robo);
    static void by_sphere(Robo* robo, const Robo* opponent);
    static void by_segment(Robo* robo);
    static void by_segment(Robo* robo, const Wall* wall);
};

#endif
