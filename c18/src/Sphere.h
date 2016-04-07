#ifndef ROBOFSPHERE_H_
#define ROBOFSPHERE_H_
#include "GraphicsDatabase/Vector3.h"

using GraphicsDatabase::Vector3;

class Sphere
{
private:
    Vector3 balance_;
    double r_;

public:
    Sphere();
    Sphere(const Vector3& balance, double radius);
    ~Sphere();
    const Vector3* balance() const;
    double radius() const;
    bool does_intersect(const Sphere& other) const;
};

#endif
