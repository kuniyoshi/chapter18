#ifndef ROBOF__BULLET_H_
#define ROBOF__BULLET_H_
#include "GraphicsDatabase/Vector3.h"

class Cuboid;
class Robo;
class Segment;
class View;

using GraphicsDatabase::Vector3;

class Bullet
{
private:
    int owner_id_;
    unsigned age_;
    bool is_homing_;
    bool did_collide_;
    Vector3 previous_point_;
    Vector3 current_point_;
    Vector3 velocity_;
    const Robo* shooter_;
    const Robo* target_robo_;

public:
    Bullet();
    ~Bullet();
    void initialize(    int id,
                        const Vector3& from,
                        const Vector3& angle,
                        const Robo* shooter,
                        const Robo* opponent,
                        bool is_homing);
    bool did_collide() const;
    Vector3 angle() const;
    void burn_at(const Vector3& at);
    void draw(const View& view) const;
    bool is_owned() const;
    bool is_owned_by(int id) const;
    Cuboid locus_cuboid() const;
    Segment locus_segment() const;
    void update();

private:
    void clear_owner();
};

#endif
