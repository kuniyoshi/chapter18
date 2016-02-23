#ifndef ROBOF_VIEW_H_
#define ROBOF_VIEW_H_
#include "Vector3.h"

class Matrix44;
class Robo;

class View
{
private:
    double near_clip_;
    double far_clip_;
    double angle_yz_;
    double angle_zx_;
    double angle_of_view_;
    double delta_angle_zx_;
    int width_;
    int height_;
    Vector3 position_;

public:
    View(int width, int height, double near_clip, double far_clip);
    ~View();
    double near_clip() const;
    double far_clip() const;
    void decrease_angle_of_view(int a);
    void draw_debug(int row) const;
    void draw_debug_string(const double* vertex, int row) const;
    void follow(const Robo& robo);
    void increase_angle_of_view(int a);
    Matrix44 get_perspective_matrix() const;
    void rotate(const Vector3& diff);
    void transform(Vector3* vertex) const;
};

#endif
