#ifndef ROBOF__BULLET_H_
#define ROBOF__BULLET_H_
#include "GraphicsDatabase/Vector3.h"

class Robo;
class View;

using GraphicsDatabase::Vector3;

class Bullet
{
private:
    int owner_id_;
    unsigned age_;
    bool is_homing_;
    Vector3 current_point_;
    Vector3 velocity_;
    const Robo* target_robo_;

public:
    Bullet();
    ~Bullet();
    void initialize(    int id,
                        const Vector3& from,
                        const Vector3& angle,
                        const Robo* opponent,
                        bool is_homing);
    Vector3 angle() const;
    void draw(const View& view) const;
    bool is_owned() const;
    bool is_owned_by(int id) const;
    void update();
};

#endif
