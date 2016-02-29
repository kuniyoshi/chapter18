#ifndef ROBOF__BULLET_H_
#define ROBOF__BULLET_H_
#include "GraphicsDatabase/Vector3.h"

class View;

using GraphicsDatabase::Vector3;

class Bullet
{
private:
    int id_;
    unsigned age_;
    bool is_owned_;
    Vector3 current_point_;
    Vector3 direction_;

public:
    Bullet();
    ~Bullet();
    void initialize(int id, const Vector3& from, const Vector3& angle);
    bool is_owned() const;
    Vector3 angle() const;
    bool is_owned_by(int id) const;
    void draw(const View& view) const;
    void update();
};

#endif
