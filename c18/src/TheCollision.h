#ifndef ROBOF__THE_COLLISION_H_
#define ROBOF__THE_COLLISION_H_

class Robo;
class Wall;

class TheCollision
{
public:
    static void slide_next_move_if_collision_will_occur(Robo* robo);
    static void slide_next_move_if_collision_will_occur(    Robo* robo,
                                                            Robo* opponent);
    static void slide_next_move_if_collision_will_occur(    Robo* robo,
                                                            Wall* wall);

private:
    static void by_cuboid(Robo* robo);
    static void by_cuboid(Robo* robo, Robo* opponent);
    static void by_sphere(Robo* robo);
    static void by_sphere(Robo* robo, Robo* opponent);
    static void by_segment(Robo* robo);
    static void by_segment(Robo* robo, Wall* wall);
};

#endif
