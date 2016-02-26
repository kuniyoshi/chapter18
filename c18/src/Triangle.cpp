#include "Triangle.h"
#include <sstream>
#include "GraphicsDatabase/Vector3.h"

using GraphicsDatabase::Vector3;

Triangle::Triangle(const Vector3& q0, const Vector3& q1, const Vector3& q2)
: p0(q0), p1(q1), p2(q2)
{}

Triangle::~Triangle() {}

void Triangle::print(std::ostringstream* oss) const
{
    *oss << "{";
    *oss << p0.x;
    *oss << ", ";
    *oss << p0.y;
    *oss << ", ";
    *oss << p0.z;
    *oss << "}, ";

    *oss << "{";
    *oss << p1.x;
    *oss << ", ";
    *oss << p1.y;
    *oss << ", ";
    *oss << p1.z;
    *oss << "}, ";

    *oss << "{";
    *oss << p2.x;
    *oss << ", ";
    *oss << p2.y;
    *oss << ", ";
    *oss << p2.z;
    *oss << "}";
}
