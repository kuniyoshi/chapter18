#include "Robo.h"
#include <sstream>
#include <string>
#include <vector>
#include "GameLib/Framework.h"
#include "GameLib/Math.h"
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Model.h"
#include "GraphicsDatabase/Vector3.h"
#include "Ai/TheArmoury.h"
#include "Cuboid.h"
#include "Segment.h"
#include "Sphere.h"
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
:   id_(id),
    model_(0),
    force_(),
    velocity_(),
    delta_next_position_(),
    angle_zx_(0.0),
    mass_(TheMass)
{
    TheDatabase::instance().create(id_, "robo");
    model_ = TheDatabase::instance().find(id_);
    delta_next_position_.copy_from(*(model_->position()));
}

Robo::~Robo()
{
    model_ = 0; // will be deleted by the database
}

const std::string& Robo::id() const { return id_; }

int Robo::int_id() const
{
    if (id_ == "myrobo")
    {
        return 0;
    }
    else if (id_ == "opponent")
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

const Vector3* Robo::force() const
{
    return &force_;
}

void Robo::force(const Vector3& new_value)
{
    force_ = new_value;
}

const Vector3* Robo::velocity() const
{
    return &velocity_;
}

void Robo::velocity(const Vector3& new_value)
{
    velocity_ = new_value;
}

const Vector3* Robo::delta_next_position() const
{
    return &delta_next_position_;
}

void Robo::delta_next_position(const Vector3& new_value)
{
    delta_next_position_ = new_value;
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

Cuboid Robo::cuboid() const
{
    const Vector3* vertexes = model_->vertexes();
    size_t size = model_->vertexes_size();
    Vector3 half_size;

    // model has not been translated, it keeps vertexes (0, 0, 0) coordinate

    for (size_t i = 0; i < size; ++i)
    {
        double dx = get_abs(vertexes[i].x);
        double dy = get_abs(vertexes[i].y);
        double dz = get_abs(vertexes[i].z);

        if (dx > half_size.x)
        {
            half_size.x = dx;
        }

        if (dy > half_size.y)
        {
            half_size.y = dy;
        }

        if (dz > half_size.z)
        {
            half_size.z = dz;
        }
    }

    Vector3 next_position(*(model_->position()));
    next_position.add(delta_next_position_);

    return Cuboid(next_position, half_size);
}

void Robo::commit_next_position()
{
    Vector3 next_position(*(model_->position()));
    next_position.add(delta_next_position_);
    model_->position(next_position);
}

const Vector3* Robo::center() const { return model_->position(); }

void Robo::draw(const View& view) const
{
    model_->draw(view.get_perspective_matrix());
}

void Robo::fire_bullet(const Vector3& angle)
{
    // Vector3 modified_angle(-angle.x, -angle.y, -angle.z);
    // modified_angle.multiply(0.3); // TODO: need logic
    // modified_angle.y = modified_angle.y + angle_zx_;
    Matrix44 rotation;
    rotation.rotate(Vector3(0.0, angle_zx_, 0.0));
    Vector3 modified_angle(angle.x, 0.0, 0.0);
    rotation.multiply(&modified_angle);
    modified_angle.y = modified_angle.y + angle_zx_;
    Ai::TheArmoury::instance().fire(*this, *model_->position(), modified_angle);
}

void Robo::print(std::ostringstream* oss) const
{
    // const Vector3* balance = model_->position();
    // *oss << "{";
    // *oss << static_cast< int >(balance->x * 100);
    // *oss << ", ";
    // *oss << static_cast< int >(balance->y * 100);
    // *oss << ", ";
    // *oss << static_cast< int >(balance->z * 100);
    // *oss << "}, ";

    // *oss << "{";
    // *oss << static_cast< int >(velocity_.x * 100);
    // *oss << ", ";
    // *oss << static_cast< int >(velocity_.y * 100);
    // *oss << ", ";
    // *oss << static_cast< int >(velocity_.z * 100);
    // *oss << "}";

    *oss << angle_zx_;
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

Segment Robo::segment() const
{
    Vector3 next_position(*(model_->position()));
    next_position.add(delta_next_position_);
    Vector3 to(next_position);
    to.add(Vector3(0.0, -1.0, 0.0));
    return Segment(next_position, to);
}

std::vector< Segment > Robo::segments() const
{
    std::vector< Segment > segments;
    segments.reserve(2);
    Vector3 balance(*(model_->position()));
    balance.add(delta_next_position_);

    Vector3 x0(balance);
    x0.x = x0.x - 0.25;
    Vector3 x1(balance);
    x1.x = x1.x + 0.25;
    segments.push_back(Segment(x0, x1));

    Vector3 y0(balance);
    y0.y = y0.y - 0.88;
    Vector3 y1(balance);
    y1.y = y1.y + 0.88;
    segments.push_back(Segment(y0, y1));

    Vector3 z0(balance);
    z0.z = z0.z - 0.25;
    Vector3 z1(balance);
    z1.z = z1.z + 0.25;
    segments.push_back(Segment(z0, z1));

    return segments;
}

void Robo::set_model_angle_zx(double new_value)
{
    angle_zx_ = new_value;
    Vector3 angle(*model_->angle());
    angle.y = new_value;
    model_->angle(angle);
}

void Robo::set_delta_next_position()
{
    TheTime t = TheTime::instance();
    double dt = static_cast< double >(t.delta()) / 1000.0;

    stop_if_too_small(&velocity_, force_);

    const Vector3* balance = model_->position();

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

    delta_next_position_ = delta;

    force_.set(0.0, -GravityAcceleration * mass_, 0.0);
}

Sphere Robo::sphere() const
{
    Vector3 next_position(*(model_->position()));
    next_position.add(delta_next_position_);
    return Sphere(next_position, 1.0); // Cheating.  This is defined at json
}

void Robo::warp(const Vector3& to)
{
    model_->position(to);
}
