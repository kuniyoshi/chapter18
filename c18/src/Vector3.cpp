#include "Vector3.h"
#include <cmath>

Vector3::Vector3()
: x(0), y(0), z(0), w(1.0)
{}

Vector3::Vector3(   double new_x,
                    double new_y,
                    double new_z,
                    double new_w)
: x(new_x), y(new_y), z(new_z), w(new_w)
{}

Vector3::Vector3(const Vector3& base)
: x(base.x), y(base.y), z(base.z), w(base.w)
{}

Vector3::~Vector3() {}

double& Vector3::operator[](int index) { return (&x)[index]; }

Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z, w);
}

void Vector3::add(const Vector3& operand)
{
    x = x + operand.x;
    y = y + operand.y;
    z = z + operand.z;
}

void Vector3::be_reciprocal()
{
    x = 1.0 / x;
    y = 1.0 / y;
    z = 1.0 / z;
}

void Vector3::clear()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

void Vector3::divide(double a)
{
    x = x / a;
    y = y / a;
    z = z / a;
}

void Vector3::hadamard_product(const Vector3& operand)
{
    x = x * operand.x;
    y = y * operand.y;
    z = z * operand.z;
}

double Vector3::length() const
{
    return std::sqrt(std::pow(x, 2.0) + std::pow(y, 2.0) + std::pow(z, 2.0));
}

void Vector3::multiply(double a)
{
    x = a * x;
    y = a * y;
    z = a * z;
}

void Vector3::normalize(double max)
{
    double total = length() / max;
    x = x / total;
    y = y / total;
    z = z / total;
}

void Vector3::power(double n)
{
    x = std::pow(x, n);
    y = std::pow(y, n);
    z = std::pow(z, n);
}

void Vector3::scale(double a)
{
    x = a * x;
    y = a * y;
    z = a * z;
}

void Vector3::set(double new_x, double new_y, double new_z, double new_w)
{
    x = new_x;
    y = new_y;
    z = new_z;
    w = new_w;
}

void Vector3::set_xy(double new_x, double new_y)
{
    x = new_x;
    y = new_y;
}

void Vector3::subtract(const Vector3& operand)
{
    x = x - operand.x;
    y = y - operand.y;
    z = z - operand.z;
}

/*
#include "Vector3.h"

Vector3::Vector3(){}

Vector3::Vector3( double aX, double aY, double aZ ) : 
x( aX ), 
y( aY ),
z( aZ ){
}

Vector3::Vector3( const Vector3& a ) :
x( a.x ),
y( a.y ),
z( a.z ){
}

void Vector3::set( double aX, double aY, double aZ ){
    x = aX;
    y = aY;
    z = aZ;
}

void Vector3::operator+=( const Vector3& a ){
    x += a.x;
    y += a.y;
    z += a.z;
}

void Vector3::operator-=( const Vector3& a ){
    x -= a.x;
    y -= a.y;
    z -= a.z;
}

void Vector3::operator=( const Vector3& a ){
    x = a.x;
    y = a.y;
    z = a.z;
}

void Vector3::setAdd( const Vector3& a, const Vector3& b ){
    x = a.x + b.x;
    y = a.y + b.y;
    z = a.z + b.z;
}

void Vector3::setSub( const Vector3& a, const Vector3& b ){
    x = a.x - b.x;
    y = a.y - b.y;
    z = a.z - b.z;
}
*/
