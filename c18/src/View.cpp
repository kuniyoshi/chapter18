#include "View.h"
#include <cmath>
#include <sstream>
#include "GameLib/Framework.h"
#include "Matrix44.h"
#include "Robo.h"
#include "Vector3.h"

namespace
{

const Vector3 FollowPoint(0.0, 1.2, -0.9);
const double FirstPositionX = 0.0;
const double FirstPositionY = 1.6;
const double FirstPositionZ = 0.0;
const int AngleScale = 3;

} // namespace -

View::View(int width, int height, double near_clip, double far_clip)
:   near_clip_(near_clip), far_clip_(far_clip),
    width_(width), height_(height),
    angle_yz_(0.0), angle_zx_(0.0),
    angle_of_view_(90.0), delta_angle_zx_(0.0),
    position_(FirstPositionX, FirstPositionY, FirstPositionZ)
{}

View::~View() {}

double View::near_clip() const { return near_clip_; }

double View::far_clip() const { return far_clip_; }

void View::decrease_angle_of_view(int a)
{
    angle_of_view_ = angle_of_view_ - static_cast< double >(a) / AngleScale;
}

void View::draw_debug(int row) const
{
    std::ostringstream oss;
    oss << "{";
    oss << angle_zx_;
    // oss << position_.x;
    // oss << ", ";
    // oss << position_.y;
    // oss << ", ";
    // oss << position_.z;
    oss << "}";

    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, oss.str().c_str());
    oss.str("");
}

void View::draw_debug_string(const double* vertex, int row) const
{
    std::ostringstream oss;
    oss << "{";

    for (int i = 0; i < 3; ++i)
    {
        oss << vertex[i];
        oss << ", ";
    }

    oss << vertex[3];
    oss << "}";

    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, oss.str().c_str());
    oss.str("");
}

void View::follow(const Robo& robo)
{
    Matrix44 transform;

    // follow position
    Vector3 follow_point(FollowPoint);
    transform.rotate_zx(robo.angle_zx());
    transform.multiply(&follow_point);

    Vector3 diff(robo.center());
    diff.add(follow_point);
    diff.subtract(position_);
    diff.multiply(0.4); // follow rate

    position_.add(diff);

    // follow angle
    angle_zx_ = -robo.angle_zx() + 180.0;
}

Matrix44 View::get_perspective_matrix() const
{
    Matrix44 perspective;
    perspective.translate(-position_);
    perspective.rotate_zx(angle_zx_ + delta_angle_zx_);
    perspective.rotate_yz(angle_yz_);
    perspective.perspective(    angle_of_view_,
                                width_,
                                height_,
                                near_clip_,
                                far_clip_);
    return perspective;
}

void View::increase_angle_of_view(int a)
{
    angle_of_view_ = angle_of_view_ + static_cast< double >(a) / AngleScale;
}

void View::rotate(const Vector3& diff)
{
    delta_angle_zx_ = delta_angle_zx_ + diff.x / static_cast< double >(AngleScale);
    angle_yz_ = angle_yz_ + diff.y;
}

void View::transform(Vector3* vertex) const
{
    Matrix44 view(get_perspective_matrix());

    // make coordinate be clip
    view.multiply(vertex);
}
