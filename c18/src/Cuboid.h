#ifndef ROBOF__CUBOID_H_
#define ROBOF__CUBOID_H_

namespace GraphicsDatabase { class Vector3; }

using GraphicsDatabase::Vector3;

class Cuboid
{
private:
    int half_x_, half_y_, half_z_;
    int balance_x_, balance_y_, balance_z_;

public:
    Cuboid();
    Cuboid(const Cuboid& base);
    Cuboid(const Vector3& balance, const Vector3& half_size);
    ~Cuboid();
    void set_balance(const Vector3& balance);
    void set_half_size(const Vector3& half_size);
    bool does_intersect(const Cuboid& that) const;
};

#endif
