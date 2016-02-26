#include "Segment.h"
#include <utility>
#include "GraphicsDatabase/Vector3.h"
#include "Triangle.h"

using GraphicsDatabase::Vector3;

Segment::Segment(const Vector3& p0, const Vector3& p1)
:   from(p0), to(p1)
{}

Segment::~Segment() {}

// segment: p-> = a-> + t b->
// triangle: c-> + u d-> + v e->
// normal: n->
// (c-> - p->) _|_ n-> === (c-> - a-> - t b->) dot n-> = 0
// t = ((c-> - a->) dot n->) / (b-> dot n->)
// 
// segment = triangle
// p-> = c-> + u d-> + v e->
// def: g-> = p-> - c->
// g-> = u d-> + v e->
// g-> dot d-> = u d-> dot d-> + v e-> dot d->
// g-> dot e-> = u d-> dot e-> + v e-> dot e->
// def: g-> dot d-> = GD
// GD * DE = u DD * DE + v ED * DE
// GE * DD = u DE * DD + v EE * DD
// GD * DE - GE * DD = v(ED * DE - EE * DD)
// v = (GD * DE - GE * DD) / (ED * DE - EE * DD)
// GD * EE = u DD * EE + v ED * EE
// GE * ED = u DE * ED + v EE * ED
// GD * EE - GE * ED = u(DD * EE - DE * ED)
// u = (GD * EE - GE * ED) / (DD * EE - DE * ED)
std::pair< bool, Vector3 >
Segment::get_intersected_point(const Triangle& triangle) const
{
    Vector3 intersected_point(0.0, 0.0, 0.0);

    const Vector3& a = from;

    Vector3 b(to);
    b.subtract(a);

    const Vector3& c = triangle.p0;

    Vector3 d(triangle.p1);
    d.subtract(triangle.p0);

    Vector3 e(triangle.p2);
    e.subtract(triangle.p0);

    Vector3 n(d);
    n.cross_product(e);

    const double bn = b.dot(n);

    if (bn == 0.0) // this means parallel
    {
        return std::pair< bool, Vector3 >(false, intersected_point);
    }

    Vector3 ac(c);
    ac.subtract(a);

    const double t = ac.dot(n) / bn;

    if (t < 0.0 || t > 1.0)
    {
        return std::pair< bool, Vector3 >(false, intersected_point);
    }

    Vector3 p(b);
    p.multiply(t);
    p.add(a);

    intersected_point = p; // do not confuse when got <false, p>, it is false

    Vector3 g(p);
    g.subtract(c);

    const double dd = d.dot(d);
    const double de = d.dot(e);
    const double ed = e.dot(d);
    const double ee = e.dot(e);
    const double gd = g.dot(d);
    const double ge = g.dot(e);

    // skip zero dividing test, denominator should not be zero
    const double u = (gd * ee - ge * ed) / (dd * ee - de * ed);

    if (u < 0.0 || u > 1.0)
    {
        return std::pair< bool, Vector3 >(false, intersected_point);
    }

    const double v = (gd * de - ge * dd) / (ed * de - ee * dd);

    const double uv = u + v;

    if (v < 0.0 || uv > 1.0)
    {
        return std::pair< bool, Vector3 >(false, intersected_point);
    }

    return std::pair< bool, Vector3 >(true, intersected_point);
}
