#ifndef ROBOF__BULLET_H_
#define ROBOF__BULLET_H_
#include "GraphicsDatabase/Vector3.h"

class View;

using GraphicsDatabase::Vector3;

class Bullet
{
private:
    int owner_id_;
    unsigned age_;
    Vector3 current_point_;
    Vector3 velocity_;

public:
    Bullet();
    ~Bullet();
    void initialize(int id, const Vector3& from, const Vector3& angle);
    Vector3 angle() const;
    void draw(const View& view) const;
    bool is_owned() const;
    bool is_owned_by(int id) const;
    void update();
};

#endif
