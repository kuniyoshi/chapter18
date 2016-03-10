#ifndef ROBOF__VIEW_H_
#define ROBOF__VIEW_H_
#include <sstream>
#include "GraphicsDatabase/Camera.h"
#include "GraphicsDatabase/Vector3.h"

namespace GraphicsDatabase { class Matrix44; }
class Robo;

using GraphicsDatabase::Matrix44;
using GraphicsDatabase::Vector3;

class View
{
private:
    GraphicsDatabase::Camera camera_;
    Vector3 delta_angle_;

public:
    View(int width, int height, double near_clip, double far_clip);
    ~View();
    double near_clip() const;
    double far_clip() const;
    const Vector3* angle() const;
    const Vector3* center() const;
    void decrease_angle_of_view(int a);
    void follow(const Robo& robo);
    Matrix44 get_perspective_matrix() const;
    void increase_angle_of_view(int a);
    void print(std::ostringstream* oss) const;
    void rotate(const Vector3& diff);
};

#endif
