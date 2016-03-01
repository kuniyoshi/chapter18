#include "Bullet.h"
#include <cassert>
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Model.h"
#include "GraphicsDatabase/Vector3.h"
#include "GameLib/Math.h"
#include "TheDatabase.h"
#include "TheTime.h"
#include "View.h"

using GraphicsDatabase::Vector3;

namespace
{

const unsigned MaxAge   = 10000;
const double Scale      = 0.1;
const double MeterPerUs = 100 / 1.0e3;

} // namespace -

Bullet::Bullet()
:   id_(-1), age_(0), is_owned_(false), current_point_(), direction_()
{}

Bullet::~Bullet() {}

void Bullet::initialize(int id, const Vector3& from, const Vector3& angle)
{
    id_ = id;
    age_ = 0;
    is_owned_ = true;
    current_point_ = from;
    Matrix44 rotation;
    rotation.rotate(angle);
    direction_ = Vector3(0.0, 0.0, 1.0);
    rotation.multiply(&direction_);
    // direction_ = Vector3(0.0, 0.0, -1.0);
}

bool Bullet::is_owned() const { return is_owned_; }

Vector3 Bullet::angle() const
{
    assert(is_owned_);
    Vector3 angle;
    angle.x = GameLib::atan2(direction_.z, direction_.y);
    angle.y = GameLib::atan2(direction_.x, direction_.z);
    angle.z = GameLib::atan2(direction_.y, direction_.x);
    return angle;
}

bool Bullet::is_owned_by(int id) const { return id == id_; }

void Bullet::draw(const View& view) const
{
    if (!is_owned_)
    {
        return;
    }

    GraphicsDatabase::Model* model = TheDatabase::instance().find("bullet");
    model->scale(Scale);
    model->angle(angle());
    model->position(current_point_);
    model->draw(view.get_perspective_matrix());
}

void Bullet::update()
{
    if (!is_owned_)
    {
        return;
    }

    unsigned delta = TheTime::instance().delta();
    age_ = age_ + delta;
    Vector3 delta_point(direction_);
    delta_point.multiply(static_cast< double >(delta) * MeterPerUs);
    current_point_.add(delta_point);

    if (age_ > MaxAge)
    {
        is_owned_ = false;
    }
}
