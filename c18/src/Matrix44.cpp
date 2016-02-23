#include "Matrix44.h"
#include "GameLib/Math.h"
#include "Vector3.h"

Matrix44::Matrix44()
:   a0_(1.0), a1_(0.0), a2_(0.0), a3_(0.0),
    b0_(0.0), b1_(1.0), b2_(0.0), b3_(0.0),
    c0_(0.0), c1_(0.0), c2_(1.0), c3_(0.0),
    d0_(0.0), d1_(0.0), d2_(0.0), d3_(1.0)
{}

Matrix44::Matrix44(const Matrix44& base)
:   a0_(base.a0_), a1_(base.a1_), a2_(base.a2_), a3_(base.a3_),
    b0_(base.b0_), b1_(base.b1_), b2_(base.b2_), b3_(base.b3_),
    c0_(base.c0_), c1_(base.c1_), c2_(base.c2_), c3_(base.c3_),
    d0_(base.d0_), d1_(base.d1_), d2_(base.d2_), d3_(base.d3_)
{}

Matrix44::~Matrix44() {}

namespace
{

double a_dot(   double a, double b, double c, double d,
                double i, double j, double k, double l)
{
    return a * i + b * j + c * k + d * l;
}

} // namespace -

void Matrix44::dot(const Matrix44& o)
{
    double ta, tb, tc, td;

    ta = a0_;
    tb = b0_;
    tc = c0_;
    td = d0_;
    a0_ = a_dot(o.a0_, o.a1_, o.a2_, o.a3_, ta, tb, tc, td);
    b0_ = a_dot(o.b0_, o.b1_, o.b2_, o.b3_, ta, tb, tc, td);
    c0_ = a_dot(o.c0_, o.c1_, o.c2_, o.c3_, ta, tb, tc, td);
    d0_ = a_dot(o.d0_, o.d1_, o.d2_, o.d3_, ta, tb, tc, td);

    ta = a1_;
    tb = b1_;
    tc = c1_;
    td = d1_;
    a1_ = a_dot(o.a0_, o.a1_, o.a2_, o.a3_, ta, tb, tc, td);
    b1_ = a_dot(o.b0_, o.b1_, o.b2_, o.b3_, ta, tb, tc, td);
    c1_ = a_dot(o.c0_, o.c1_, o.c2_, o.c3_, ta, tb, tc, td);
    d1_ = a_dot(o.d0_, o.d1_, o.d2_, o.d3_, ta, tb, tc, td);

    ta = a2_;
    tb = b2_;
    tc = c2_;
    td = d2_;
    a2_ = a_dot(o.a0_, o.a1_, o.a2_, o.a3_, ta, tb, tc, td);
    b2_ = a_dot(o.b0_, o.b1_, o.b2_, o.b3_, ta, tb, tc, td);
    c2_ = a_dot(o.c0_, o.c1_, o.c2_, o.c3_, ta, tb, tc, td);
    d2_ = a_dot(o.d0_, o.d1_, o.d2_, o.d3_, ta, tb, tc, td);

    ta = a3_;
    tb = b3_;
    tc = c3_;
    td = d3_;
    a3_ = a_dot(o.a0_, o.a1_, o.a2_, o.a3_, ta, tb, tc, td);
    b3_ = a_dot(o.b0_, o.b1_, o.b2_, o.b3_, ta, tb, tc, td);
    c3_ = a_dot(o.c0_, o.c1_, o.c2_, o.c3_, ta, tb, tc, td);
    d3_ = a_dot(o.d0_, o.d1_, o.d2_, o.d3_, ta, tb, tc, td);
}

// | a0_ a1_ a2_ a3_ | |px|
// | b0_ b1_ b2_ b3_ | |py|
// | c0_ c1_ c2_ c3_ | |pz|
// | d0_ d1_ d2_ d3_ | |pw|
void Matrix44::multiply(Vector3* operand) const
{
    const double x = operand->x;
    const double y = operand->y;
    const double z = operand->z;
    const double w = operand->w;

    operand->x = a0_ * x + a1_ * y + a2_ * z + a3_ * w;
    operand->y = b0_ * x + b1_ * y + b2_ * z + b3_ * w;
    operand->z = c0_ * x + c1_ * y + c2_ * z + c3_ * w;
    operand->w = d0_ * x + d1_ * y + d2_ * z + d3_ * w;
}

// def. Z = (a + bz) / -z
// 0 = (a + -nb) / +n
// 1 = (a + -fb) / +f
// f = (n - f)b
// b = f / (n - f)
// a = nb
//
// | sk 0   0   0 | | a0_ a1_ a2_ a3_ |
// | 0  s   0   0 | | b0_ b1_ b2_ b3_ |
// | 0  0   b   a | | c0_ c1_ c2_ c3_ |
// | 0  0   -1  0 | | d0_ d1_ d2_ d3_ |
void Matrix44::perspective( double theta,
                            int width,
                            int height,
                            double near_clip,
                            double far_clip)
{
    const double c0 = c0_;
    const double c1 = c1_;
    const double c2 = c2_;
    const double c3 = c3_;

    const double s = 1.0 / GameLib::tan(theta / 2.0);
    const double k = static_cast< double >(height) / width;
    const double b = far_clip / (near_clip - far_clip);
    const double a = near_clip * b;

    a0_ = s * k * a0_;
    a1_ = s * k * a1_;
    a2_ = s * k * a2_;
    a3_ = s * k * a3_;

    b0_ = s * b0_;
    b1_ = s * b1_;
    b2_ = s * b2_;
    b3_ = s * b3_;

    c0_ = b * c0_ + a * d0_;
    c1_ = b * c1_ + a * d1_;
    c2_ = b * c2_ + a * d2_;
    c3_ = b * c3_ + a * d3_;

    d0_ = -c0;
    d1_ = -c1;
    d2_ = -c2;
    d3_ = -c3;
}

// |x|   | 1    0           0           0 | |px|
// |y| = | 0    cos(theta)  -sin(theta) 0 | |py|
// |z|   | 0    sin(theta)  cos(theta)  0 | |pz|
// |w|   | 0    0           0           1 | |pw|
//
// | 1    0           0           0 | | a0_ a1_ a2_ a3_ |
// | 0    cos(theta)  -sin(theta) 0 | | b0_ b1_ b2_ b3_ |
// | 0    sin(theta)  cos(theta)  0 | | c0_ c1_ c2_ c3_ |
// | 0    0           0           1 | | d0_ d1_ d2_ d3_ |
void Matrix44::rotate_yz(double angle)
{
    const double cosine = GameLib::cos(angle);
    const double sine = GameLib::sin(angle);

    const double b0 = b0_;
    const double b1 = b1_;
    const double b2 = b2_;
    const double b3 = b3_;

    b0_ = cosine * b0 - sine * c0_;
    b1_ = cosine * b1 - sine * c1_;
    b2_ = cosine * b2 - sine * c2_;
    b3_ = cosine * b3 - sine * c3_;

    c0_ = sine * b0 + cosine * c0_;
    c1_ = sine * b1 + cosine * c1_;
    c2_ = sine * b2 + cosine * c2_;
    c3_ = sine * b3 + cosine * c3_;
}

// |x|   | cos(theta)  0   sin(theta)  0 | |px|
// |y| = | 0           1   0           0 | |py|
// |z|   | -sin(theta) 0   cos(theta)  0 | |pz|
// |w|   | 0           0   0           1 | |pw|
//
// | cos(theta)    0   sin(theta)  0 | | a0_ a1_ a2_ a3_ |
// | 0             1   0           0 | | b0_ b1_ b2_ b3_ |
// | -sin(theta)   0   cos(theta)  0 | | c0_ c1_ c2_ c3_ |
// | 0             0   0           1 | | d0_ d1_ d2_ d3_ |
void Matrix44::rotate_zx(double angle)
{
    const double cosine = GameLib::cos(angle);
    const double sine = GameLib::sin(angle);

    const double a0 = a0_;
    const double a1 = a1_;
    const double a2 = a2_;
    const double a3 = a3_;

    a0_ = cosine * a0 + sine * c0_;
    a1_ = cosine * a1 + sine * c1_;
    a2_ = cosine * a2 + sine * c2_;
    a3_ = cosine * a3 + sine * c3_;

    c0_ = -sine * a0 + cosine * c0_;
    c1_ = -sine * a1 + cosine * c1_;
    c2_ = -sine * a2 + cosine * c2_;
    c3_ = -sine * a3 + cosine * c3_;
}

// | 1  0   0   delta.x | | a0_ a1_ a2_ a3_ |
// | 0  1   0   delta.y | | b0_ b1_ b2_ b3_ |
// | 0  0   1   delta.z | | c0_ c1_ c2_ c3_ |
// | 0  0   0   1       | | d0_ d1_ d2_ d3_ |
void Matrix44::translate(const Vector3& delta)
{
    a0_ = a0_ + delta.x * d0_;
    a1_ = a1_ + delta.x * d1_;
    a2_ = a2_ + delta.x * d2_;
    a3_ = a3_ + delta.x * d3_;

    b0_ = b0_ + delta.y * d0_;
    b1_ = b1_ + delta.y * d1_;
    b2_ = b2_ + delta.y * d2_;
    b3_ = b3_ + delta.y * d3_;

    c0_ = c0_ + delta.z * d0_;
    c1_ = c1_ + delta.z * d1_;
    c2_ = c2_ + delta.z * d2_;
    c3_ = c3_ + delta.z * d3_;
}


// | a  0   0   0 | | a0_ a1_ a2_ a3_ |
// | 0  a   0   0 | | b0_ b1_ b2_ b3_ |
// | 0  0   1   0 | | c0_ c1_ c2_ c3_ |
// | 0  0   0   1 | | d0_ d1_ d2_ d3_ |
void Matrix44::scale(double a)
{
    a0_ = a * a0_;
    a1_ = a * a1_;
    a2_ = a * a2_;
    a3_ = a * a3_;
    b0_ = a * b0_;
    b1_ = a * b1_;
    b2_ = a * b2_;
    b3_ = a * b3_;
}

/*
#include "Matrix34.h"
#include "Vector3.h"
#include "GameLib/Math.h"
using namespace GameLib;

Matrix34::Matrix34(){
}

void Matrix34::setTranslation( const Vector3& a ){
    m00 = m11 = m22 = 1.0;
    m01 = m10 = m20 = m02 = m12 = m21 = 0.0;
    m03 = a.x;
    m13 = a.y;
    m23 = a.z;
}

void Matrix34::setScaling( const Vector3& a ){
    m00 = a.x;
    m11 = a.y;
    m22 = a.z;
    m01 = m10 = m02 = m20 = m12 = m21 = 0.0;
    m03 = m13 = m23 = 0.0;
}

void Matrix34::setRotationX( double r ){
    double c = cos( r );
    double s = sin( r );
    m11 = m22 = c;
    m12 = -s;
    m21 = s;
    m00 = 1.0;
    m01 = m02 = m10 = m20 = 0.0;
    m03 = m13 = m23 = 0.0;
}

void Matrix34::setRotationY( double r ){
    double c = cos( r );
    double s = sin( r );
    m00 = m22 = c;
    m20 = -s;
    m02 = s;
    m11 = 1.0;
    m01 = m10 = m12 = m21 = 0.0;
    m03 = m13 = m23 = 0.0;
}

void Matrix34::setRotationZ( double r ){
    double c = cos( r );
    double s = sin( r );
    m00 = m11 = c;
    m01 = -s;
    m10 = s;
    m22 = 1.0;
    m02 = m12 = m20 = m21 = 0.0;
    m03 = m13 = m23 = 0.0;
}

void Matrix34::operator*=( const Matrix34& m ){
    double tx, ty, tz;
    tx = m00; ty = m01; tz = m02;
    m00 = tx * m.m00 + ty * m.m10 + tz * m.m20;
    m01 = tx * m.m01 + ty * m.m11 + tz * m.m21;
    m02 = tx * m.m02 + ty * m.m12 + tz * m.m22;
    m03 = tx * m.m03 + ty * m.m13 + tz * m.m23 + m03;
    tx = m10; ty = m11; tz = m12;
    m10 = tx * m.m00 + ty * m.m10 + tz * m.m20;
    m11 = tx * m.m01 + ty * m.m11 + tz * m.m21;
    m12 = tx * m.m02 + ty * m.m12 + tz * m.m22;
    m13 = tx * m.m03 + ty * m.m13 + tz * m.m23 + m13;
    tx = m20; ty = m21; tz = m22;
    m20 = tx * m.m00 + ty * m.m10 + tz * m.m20;
    m21 = tx * m.m01 + ty * m.m11 + tz * m.m21;
    m22 = tx * m.m02 + ty * m.m12 + tz * m.m22;
    m23 = tx * m.m03 + ty * m.m13 + tz * m.m23 + m23;
}

void Matrix34::multiply( Vector3* out, const Vector3& in ) const {
    double tx = in.x; 
    double ty = in.y; 
    out->x = m00 * tx + m01 * ty + m02 * in.z + m03;
    out->y = m10 * tx + m11 * ty + m12 * in.z + m13;
    out->z = m20 * tx + m21 * ty + m22 * in.z + m23;
}

void Matrix34::translate( const Vector3& a ){
    m03 += m00 * a.x + m01 * a.y + m02 * a.z;
    m13 += m10 * a.x + m11 * a.y + m12 * a.z;
    m23 += m20 * a.x + m21 * a.y + m22 * a.z;
}

void Matrix34::scale( const Vector3& a ){
    m00 *= a.x;
    m01 *= a.y;
    m02 *= a.z;
    m10 *= a.x;
    m11 *= a.y;
    m12 *= a.z;
    m20 *= a.x;
    m21 *= a.y;
    m22 *= a.z;
}

void Matrix34::rotateX( double r ){
    double c = cos( r );
    double s = sin( r );
    double t;
    t = c * m01 + s * m02;
    m02 = -s * m01 + c * m02;
    m01 = t;
    t = c * m11 + s * m12;
    m12 = -s * m11 + c * m12;
    m11 = t;
    t = c * m21 + s * m22;
    m22 = -s * m21 + c * m22;
    m21 = t;
}

void Matrix34::rotateY( double r ){
    double c = cos( r );
    double s = sin( r );
    double t;
    t = c * m00 - s * m02;
    m02 = s * m00 + c * m02;
    m00 = t;
    t = c * m10 - s * m12;
    m12 = s * m10 + c * m12;
    m10 = t;
    t = c * m20 - s * m22;
    m22 = s * m20 + c * m22;
    m20 = t;
}

void Matrix34::rotateZ( double r ){
    double c = cos( r );
    double s = sin( r );
    double t;
    t = c * m00 + s * m01;
    m01 = -s * m00 + c * m01;
    m00 = t;
    t = c * m10 + s * m11;
    m11 = -s * m10 + c * m11;
    m10 = t;
    t = c * m20 + s * m21;
    m21 = -s * m20 + c * m21;
    m20 = t;
}
*/
