#include "Robo.h"
#include <sstream>
#include "GameLib/Framework.h"
#include "GameLib/Math.h"
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Model.h"
#include "GraphicsDatabase/Vector3.h"
#include "TheDatabase.h"
#include "TheTime.h"
#include "View.h"

using GraphicsDatabase::Matrix44;
using GraphicsDatabase::Model;
using GraphicsDatabase::Vector3;

namespace
{

const double TheMass    = 100; // [kg]
const double HalfWidth  = 0.25; // [m]
const double Height     = 1.0; // [m]
const double Volume     = 2.0 * HalfWidth * 2.0 * HalfWidth * Height;
const double Density    = TheMass / Volume; // 945; // [kg/m^3]
const double DragArea   = 2.0 * HalfWidth * Height;

const double AirDensity     = 1.293; // [kg/m^3]
const double AirViscosity   = 1.8 * 1e-5;

const double GravityAcceleration = 9.8; // [m/s^2]

const double DirectionCoefSide = 0.8;
const double DirectionCoefBack = 0.6;

const int AngleScale = 3;

} // namespae -

Robo::Robo(const std::string& id)
:   model_(0),
    force_(),
    velocity_(),
    angle_zx_(0.0),
    mass_(TheMass)
{
    TheDatabase::instance().create(id, "robo");
    model_ = TheDatabase::instance().find(id);
}

Robo::~Robo()
{
    model_ = 0; // will be deleted by the database
}

double Robo::angle_zx() const { return angle_zx_; }

namespace
{

void add_force( Vector3* force,
                const double angle_zx,
                const Vector3& direction,
                const double acceleration)
{
    Vector3 a(direction);

    Matrix44 rotation;
    rotation.rotate_zx(angle_zx);
    rotation.multiply(&a);

    a.multiply(acceleration);

    a.x = DirectionCoefSide * a.x;

    if (direction.z < 0)
    {
        a.z = DirectionCoefBack * a.z;
    }

    (*force).add(a);
}

Vector3 get_drag(const Vector3& velocity)
{
    const double DragCoef = 2.0;
    Vector3 drag(velocity);
    drag.power(2.0);
    drag.multiply(AirDensity * DragArea * DragCoef / 2.0);
    return drag;
}

double get_abs(double a) { return a < 0.0 ? -a : a; }

void stop_if_too_small(Vector3* velocity, const Vector3& force)
{
    const double min_limit = 1e-3;

    if (force.x == 0.0)
    {
        (*velocity).x = get_abs((*velocity).x) < min_limit ? 0 : (*velocity).x;
    }

    if (force.z == 0.0)
    {
        (*velocity).z = get_abs((*velocity).z) < min_limit ? 0 : (*velocity).z;
    }
}

double get_reverse_direction(double a)
{
    if (a > 0.0)
    {
        return -1.0;
    }
    else if (a < 0.0)
    {
        return 1.0;
    }

    return 0.0;
}

} // namespace

// Cuboid
void Robo::boost(const Vector3& direction)
{
    Vector3 tuned_direction(direction);
    const Vector3* balance = model_->position();

    if ((balance->y > Height / 2.0) && tuned_direction.length() > 0.0)
    {
        const double up_rate = 0.4;
        tuned_direction.multiply(1.0 - up_rate);
        tuned_direction.y = up_rate;
    }

    if (tuned_direction.length() == 0.0)
    {
        tuned_direction.y = 1.0;
    }

    const double BoostAcceleration = 15.0;
    const double a = BoostAcceleration * mass_;
    add_force(&force_, angle_zx_, tuned_direction, a);
}

const Vector3* Robo::center() const { return model_->position(); }

void Robo::draw(const View& view) const
{
    model_->draw(view.get_perspective_matrix());
}

void Robo::rotate_zx(int angle_zx)
{
    angle_zx_ = angle_zx_ + static_cast< double >(angle_zx) / AngleScale;
    Vector3 angle((*model_->angle()));
    angle.y = angle_zx_;
    model_->angle(angle);
}

void Robo::run(const Vector3& direction)
{
    const double RunAcceleration = 22.0;
    const double a = RunAcceleration * mass_;
    add_force(&force_, angle_zx_, direction, a);
}

void Robo::tick()
{
    TheTime t = TheTime::instance();
    double dt = static_cast< double >(t.delta()) / 1000.0;

    stop_if_too_small(&velocity_, force_);

    const Vector3* balance = model_->position();

    if ((force_.y < 0.0 || velocity_.y < 0.0) && balance->y <= Height / 2.0)
    {
        force_.y = 0.0;
        velocity_.y = 0.0;
    }

    Vector3 drag(get_drag(velocity_));
    Vector3 friction;

    if (velocity_.y == 0.0 && balance->y <= Height / 2.0)
    {
        // Si: 0.58, Fe: 0.52
        const double FrictionCoef = (0.58 + 0.52) / 2.0;

        friction.x = get_reverse_direction(velocity_.x);
        friction.z = get_reverse_direction(velocity_.z);
        friction.multiply(mass_ * FrictionCoef);
    }

    Vector3 dv(force_);
    dv.subtract(drag);
    dv.add(friction);
    dv.multiply(dt);
    dv.divide(mass_);
    velocity_.add(dv);

    Vector3 delta(velocity_);
    delta.multiply(dt);

    Vector3 new_position(*balance);
    new_position.add(delta);

    if (new_position.y < Height / 2.0)
    {
        new_position.y = Height / 2.0;
    }

    model_->position(new_position);

    force_.set(0.0, -GravityAcceleration * mass_, 0.0);
}

void Robo::warp(const Vector3& to)
{
    model_->position(to);
}
