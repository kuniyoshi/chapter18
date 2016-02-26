#ifndef ROBOF__TRIANGLE_H_
#define ROBOF__TRIANGLE_H_
#include <sstream>
#include "GraphicsDatabase/Vector3.h"

using GraphicsDatabase::Vector3;

class Triangle
{
public:
    Vector3 p0, p1, p2;

public:
    Triangle(const Vector3& q0, const Vector3& q1, const Vector3& q2);
    ~Triangle();
    void print(std::ostringstream* oss) const;
};

#endif
