#include "View.h"
#include <cmath>
#include <sstream>
#include "GameLib/Framework.h"
#include "GraphicsDatabase/Camera.h"
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Vector3.h"
#include "Robo.h"
#include "TheTime.h"

using GraphicsDatabase::Matrix44;
using GraphicsDatabase::Vector3;

namespace
{

const Vector3 FollowPoint(0.0, 1.2, -1.2);
const Vector3 FirstPosition(0.0, 1.6, 0.0);
const Vector3 FirstAngle(0.0, 180.0, 0.0);
const Vector3 AnglePerMs(0.1, 0.1, 0.1);
const double AngleOfView        = 90.0;
const double AngleOfViewPerMs   = 0.10;

} // namespace -

View::View(int width, int height, double near_clip, double far_clip)
:   camera_(),
    delta_angle_(0.0, 0.0, 0.0)
{
    camera_.near_clip(near_clip);
    camera_.far_clip(far_clip);
    camera_.width(width);
    camera_.height(height);
    camera_.angle_of_view(AngleOfView);
    camera_.angle(FirstAngle);
    camera_.position(FirstPosition);
}

View::~View() {}

double View::near_clip() const { return camera_.near_clip(); }

double View::far_clip() const { return camera_.far_clip(); }

const Vector3* View::angle() const
{
    return camera_.angle();
}

const Vector3* View::center() const
{
    return camera_.position();
}

void View::decrease_angle_of_view(int a)
{
    unsigned delta = TheTime::instance().delta();
    double angle_of_view
    = camera_.angle_of_view() - static_cast< double >(delta) * AngleOfViewPerMs;
    camera_.angle_of_view(angle_of_view);
}

void View::follow(const Robo& robo)
{
    Matrix44 transform;

    // follow position
    Vector3 follow_point(FollowPoint);
    transform.rotate_zx(robo.angle_zx());
    transform.multiply(&follow_point);

    Vector3 current(*(camera_.position()));

    Vector3 diff(*(robo.center()));
    diff.add(follow_point);
    diff.subtract(current);
    diff.multiply(0.4); // follow rate

    current.add(diff);
    camera_.position(current);

    // follow angle
    Vector3 angle;
    angle.y = -robo.angle_zx() + 180.0;
    angle.add(delta_angle_);
    camera_.angle(angle);
}

Matrix44 View::get_perspective_matrix() const
{
    return camera_.get_perspective_matrix();
}

void View::increase_angle_of_view(int a)
{
    unsigned delta = TheTime::instance().delta();
    double angle_of_view
    = camera_.angle_of_view() + static_cast< double >(delta) * AngleOfViewPerMs;
    camera_.angle_of_view(angle_of_view);
}

void View::print(std::ostringstream* oss) const
{
    Vector3 camera_angle(*camera_.angle());
    *oss << "{";
    *oss << static_cast< int >(camera_angle.x);
    *oss << ", ";
    *oss << static_cast< int >(camera_angle.y);
    *oss << ", ";
    *oss << static_cast< int >(camera_angle.z);
    *oss << "}, ";
    *oss << "{";
    *oss << static_cast< int >(delta_angle_.x);
    *oss << ", ";
    *oss << static_cast< int >(delta_angle_.y);
    *oss << ", ";
    *oss << static_cast< int >(delta_angle_.z);
    *oss << "}";
}

void View::rotate(const Vector3& diff)
{
    Vector3 angle(diff);
    unsigned delta = TheTime::instance().delta();
    angle.hadamard_product(AnglePerMs);
    angle.multiply(static_cast< double >(delta));
    delta_angle_.add(angle);
}
