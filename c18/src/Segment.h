#ifndef ROBOF__SEGMENT_H_
#define ROBOF__SEGMENT_H_
#include <utility>
#include "GraphicsDatabase/Vector3.h"

class Triangle;

using GraphicsDatabase::Vector3;

class Segment
{
public:
    Vector3 from;
    Vector3 to;

public:
    Segment(const Vector3& p0, const Vector3& p1);
    ~Segment();
    std::pair< bool, Vector3 >
    get_intersected_point(const Triangle& triangle) const;
};

#endif
