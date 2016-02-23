#include "Sample.h"

void format_vector(double* p)
{
    p[1] = p[1] * 640.0 / 480.0;
    // p[1] = 0.0;
    p[3] = p[2];
    p[2] = (1.0 / 9999.0) * p[2] - (1.0 / 9999.0);
    p[2] = p[2] * p[3];
}

void set_vector1(double* p)
{
    p[0] = -10.0;
    p[1] = -1.0;
    p[2] = 10.0;
    p[3] = 1.0;
}

void set_vector2(double* p)
{
    p[0] = 10.0;
    p[1] = -1.0;
    p[2] = 10.0;
    p[3] = 1.0;
}

void set_vector3(double* p)
{
    p[0] = -10.0;
    p[1] = -1.0;
    p[2] = 1.0;
    p[3] = 1.0;
}

void set_vector4(double* p)
{
    p[0] = 10.0;
    p[1] = -1.0;
    p[2] = 1.0;
    p[3] = 1.0;
}