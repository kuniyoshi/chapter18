#include "Bullet.h"
#include <cassert>
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Model.h"
#include "GraphicsDatabase/Vector3.h"
#include "GameLib/Math.h"
#include "Robo.h"
#include "TheDatabase.h"
#include "TheTime.h"
#include "View.h"

using GraphicsDatabase::Vector3;
using GraphicsDatabase::Matrix44;

namespace
{

const unsigned MaxAgeMs             = 10000;
const double Scale                  = 0.2;
const double GravityAcceleration    = 9.8;
const double Speed0                 = 50.0;
const double SpeedMax               = 60.0;
// const double Speed0     = 115.0;
// const double SpeedMax   = 295.0;
const unsigned BoostAfter           = 2 * 1000;
// const unsigned BoostAfter   = 10 * 1000;
const double BoostS                 = 1.0;
// const double BoostS         = 2.0;
const unsigned BoostMs              = static_cast< unsigned >(BoostS * 1000);
const double DeltaSpeed             = (SpeedMax - Speed0) / BoostS;
const double AbsAngleLimit          = 5.0;
const double AnglePerMs             = 2.0 / 1e3;
const double ActiveAngleLimit       = 30.0;

double calc_delta_speed(unsigned from, unsigned dt)
{
    if (from + dt < BoostAfter)
    {
        return 0.0;
    }
    else if (from + dt > BoostAfter + BoostMs)
    {
        return 0.0;
    }

    // ignore the case that
    // from < BoostAfter; from + dt > BoostAfter

    return DeltaSpeed * dt / 1e3;
}

} // namespace -

Bullet::Bullet()
:   owner_id_(-1), age_(0),
    current_point_(), velocity_(),
    target_robo_(0)
{}

Bullet::~Bullet()
{
    target_robo_ = 0; // do not delete the robo, it will be done at other
}

void Bullet::initialize(    int id,
                            const Vector3& from,
                            const Vector3& angle,
                            const Robo* opponent)
{
    assert(id >= 0);
    owner_id_ = id;
    age_ = 0;
    current_point_ = from;
    Matrix44 rotation;
    rotation.rotate(angle);
    velocity_ = Vector3(0.0, 0.0, Speed0);
    rotation.multiply(&velocity_);
    target_robo_ = opponent;
}

Vector3 Bullet::angle() const
{
    assert(is_owned());
    Vector3 angle;
    angle.x = GameLib::atan2(velocity_.z, velocity_.y);
    angle.y = GameLib::atan2(velocity_.x, velocity_.z);
    angle.z = GameLib::atan2(velocity_.y, velocity_.x);
    return angle;
}

void Bullet::draw(const View& view) const
{
    if (!is_owned())
    {
        return;
    }

    GraphicsDatabase::Model* model = TheDatabase::instance().find("bullet");
    model->scale(Scale);
    model->angle(angle());
    model->position(current_point_);
    model->draw(view.get_perspective_matrix());
}

bool Bullet::is_owned() const { return owner_id_ >= 0; }

bool Bullet::is_owned_by(int id) const { return id == owner_id_; }

namespace
{

void increase_velocity( Vector3* velocity,
                        const Vector3& current_angle,
                        unsigned age,
                        unsigned dt)
{
    double ds = calc_delta_speed(age, dt);
    Vector3 delta(0.0, 0.0, ds);
    Matrix44 rotation;
    rotation.rotate(current_angle);
    rotation.multiply(&delta);
    velocity->add(delta);
}

void fix_velocity_cause_of_thruster(    Vector3* velocity,
                                        const Vector3& current_point,
                                        const Vector3& target_point,
                                        double t,
                                        double dt)
{
    Vector3 direction(target_point);
    direction.subtract(current_point);
    double angle_t = GameLib::atan2(direction.x, direction.z);
    double angle_c = GameLib::atan2(velocity->x, velocity->z);
    double delta_angle = angle_t - angle_c;

    if (delta_angle > 180.0)
    {
        delta_angle = delta_angle - 360.0;
    }
    else if (delta_angle <= -180.0)
    {
        delta_angle = delta_angle + 360.0;
    }

    if (delta_angle < -ActiveAngleLimit || delta_angle > ActiveAngleLimit)
    {
        return;
    }

    delta_angle = delta_angle * AnglePerMs / t;

    if (delta_angle > AbsAngleLimit)
    {
        delta_angle = AbsAngleLimit;
    }
    else if (delta_angle < -AbsAngleLimit)
    {
        delta_angle = -AbsAngleLimit;
    }

    Matrix44 rotation;
    rotation.rotate(Vector3(0.0, delta_angle, 0.0));
    rotation.multiply(velocity);
}

} // namespace -

void Bullet::update()
{
    if (!is_owned())
    {
        return;
    }

    unsigned dt = TheTime::instance().delta();
    increase_velocity(&velocity_, angle(), age_, dt);

    velocity_.y = velocity_.y
    - GravityAcceleration * static_cast< double >(dt) / 1e3;

    age_ = age_ + dt;

    Vector3 delta(velocity_);
    delta.multiply(static_cast< double >(dt) / 1e3);

    current_point_.add(delta);

    if (target_robo_)
    {
        fix_velocity_cause_of_thruster( &velocity_,
                                        current_point_,
                                        *target_robo_->center(),
                                        static_cast< double >(age_) / 1e3,
                                        dt);
    }

    if (age_ > MaxAgeMs)
    {
        owner_id_ = -1;
        target_robo_ = 0;
    }
}
