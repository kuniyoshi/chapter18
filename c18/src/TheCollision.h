#ifndef ROBOF__THE_COLLISION_H_
#define ROBOF__THE_COLLISION_H_

class Robo;

class TheCollision
{
public:
    static void slide_next_move_if_collision_will_occur(Robo* robo);
    static void slide_next_move_if_collision_will_occur(    Robo* robo,
                                                            Robo* opponent);

private:
    static void by_cuboid(Robo* robo);
    static void by_cuboid(Robo* robo, Robo* opponent);
    // static void by_sphere(Robo* robo);
};

#endif
