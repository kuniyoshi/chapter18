#include "Cuboid.h"
#include "GraphicsDatabase/Vector3.h"

using GraphicsDatabase::Vector3;

namespace
{

const int InternalRange = 1000;

} // namespace -

Cuboid::Cuboid()
:   balance_x_(0), balance_y_(0), balance_z_(0),
    half_x_(0), half_y_(0), half_z_(0)
{}

Cuboid::Cuboid(const Cuboid& base)
:   balance_x_(base.balance_x_),
    balance_y_(base.balance_y_),
    balance_z_(base.balance_z_),
    half_x_(base.half_x_),
    half_y_(base.half_y_),
    half_z_(base.half_z_)
{}

Cuboid::Cuboid(const Vector3& balance, const Vector3& half_size)
{
    balance_x_ = static_cast< int >(balance.x * InternalRange);
    balance_y_ = static_cast< int >(balance.y * InternalRange);
    balance_z_ = static_cast< int >(balance.z * InternalRange);
    half_x_ = static_cast< int >(half_size.x * InternalRange);
    half_y_ = static_cast< int >(half_size.y * InternalRange);
    half_z_ = static_cast< int >(half_size.z * InternalRange);
}

Cuboid::~Cuboid() {}

void Cuboid::set_balance(const Vector3& balance)
{
    balance_x_ = static_cast< int >(balance.x * InternalRange);
    balance_y_ = static_cast< int >(balance.y * InternalRange);
    balance_z_ = static_cast< int >(balance.z * InternalRange);
}

void Cuboid::set_half_size(const Vector3& half_size)
{
    half_x_ = static_cast< int >(half_size.x * InternalRange);
    half_y_ = static_cast< int >(half_size.y * InternalRange);
    half_z_ = static_cast< int >(half_size.z * InternalRange);
}

bool Cuboid::does_intersect(const Cuboid& that) const
{
    const Cuboid& b = that;
    int ax0 = balance_x_ - half_x_;
    int ax1 = balance_x_ + half_x_;
    int bx0 = b.balance_x_ - b.half_x_;
    int bx1 = b.balance_x_ + b.half_x_;

    if (ax0 < bx1 && ax1 > bx0)
    {
        int ay0 = balance_y_ - half_y_;
        int ay1 = balance_y_ + half_y_;
        int by0 = b.balance_y_ - b.half_y_;
        int by1 = b.balance_y_ + b.half_y_;

        if (ay0 < by1 && ay1 > by0)
        {
            int az0 = balance_z_ - half_z_;
            int az1 = balance_z_ + half_z_;
            int bz0 = b.balance_z_ - b.half_z_;
            int bz1 = b.balance_z_ + b.half_z_;

            if (az0 < bz1 && az1 > bz0)
            {
                return true;
            }
        }
    }

    return false;
}
