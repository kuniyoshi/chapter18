#ifndef ROBOF_VECTOR_H_
#define ROBOF_VECTOR_H_

class Vector3
{
public:
    double x, y, z, w;

public:
    Vector3();
    Vector3(double new_x, double new_y, double new_z = 0.0, double new_w = 1.0);
    Vector3(const Vector3& base);
    ~Vector3();
    double& operator[](int index);
    Vector3 operator-() const;
    void add(const Vector3& operand);
    void be_reciprocal();
    void clear();
    void divide(double a);
    void hadamard_product(const Vector3& operand);
    double length() const;
    void multiply(double a);
    void normalize(double max);
    void power(double n);
    void scale(double a);
    void set(double new_x, double new_y, double new_z, double new_w = 1.0);
    void set_xy(double new_x, double new_y);
    void subtract(const Vector3& operand);
};

#endif


/*
#ifndef INCLUDED_VECTOR3_H
#define INCLUDED_VECTOR3_H

class Vector3{
public:
    Vector3();
    Vector3( double x, double y, double z );
    Vector3( const Vector3& );
    void set( double x, double y, double z );
    void operator+=( const Vector3& );
    void operator-=( const Vector3& );
    void operator=( const Vector3& );
    void setAdd( const Vector3&, const Vector3& );
    void setSub( const Vector3&, const Vector3& );

    double x, y, z;
};

#endif
*/
