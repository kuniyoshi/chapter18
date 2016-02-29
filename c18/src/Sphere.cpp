#include "Sphere.h"
#include "GraphicsDatabase/Vector3.h"

using GraphicsDatabase::Vector3;

Sphere::Sphere()
: balance_(), r_(0.0)
{}

Sphere::Sphere(const Vector3& balance, double radius)
: balance_(balance), r_(radius)
{}

Sphere::~Sphere() {}

const Vector3* Sphere::balance() const { return &balance_; }

double Sphere::radius() const { return r_; }

bool Sphere::does_intersect(const Sphere& other) const
{
    Vector3 diff(balance_);
    diff.subtract(other.balance_);
    double rr = r_ + other.r_;

    return diff.squared_length() < rr * rr;
}
