#ifndef ROBO_F_MATRIX34
#define ROBO_F_MATRIX34

class Vector3;

class Matrix44
{
private:
    double a0_, a1_, a2_, a3_;
    double b0_, b1_, b2_, b3_;
    double c0_, c1_, c2_, c3_;
    double d0_, d1_, d2_, d3_;

public:
    Matrix44();
    Matrix44(const Matrix44& base);
    ~Matrix44();
    void dot(const Matrix44& operand);
    void multiply(Vector3* operand) const;
    void perspective(   double theta,
                        int width,
                        int heigt,
                        double near_clip,
                        double far_clip);
    void rotate_yz(double angle_yz);
    void rotate_zx(double angle_zx);
    void scale(double a);
    void translate(const Vector3& delta);
};

#endif

/*
#ifndef INCLUDED_MATRIX34_H
#define INCLUDED_MATRIX34_H

class Vector3;

class Matrix34{
public:
    Matrix34();
    void setTranslation( const Vector3& );
    void setScaling( const Vector3& );
    void setRotationX( double radian );
    void setRotationY( double radian );
    void setRotationZ( double radian );
    void multiply( Vector3* out, const Vector3& in ) const;
    void operator*=( const Matrix34& );
    void translate( const Vector3& );
    void scale( const Vector3& );
    void rotateX( double radian );
    void rotateY( double radian );
    void rotateZ( double radian );
private:
    double m00, m01, m02, m03;
    double m10, m11, m12, m13;
    double m20, m21, m22, m23;
};

#endif
*/
