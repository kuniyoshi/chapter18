#include "Robo.h"
#include <cstdlib>
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
#include "TheEnvironment.h"
#include "TheTime.h"
#include "Triangle.h"
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

const double DirectionCoefSide = 0.8;
const double DirectionCoefBack = 0.6;

const int AngleScale = 3;

const unsigned ChargingMs = 200;

const double AbsorptionEnergyPerMs  = 0.001;
const double BoostEnergyPerMs       = 0.0004;

} // namespae -

Robo::Robo(const std::string& id)
:   id_(id),
    model_(0),
    force_(),
    velocity_(),
    delta_next_position_(),
    angle_zx_(0.0),
    mass_(TheMass),
    view_(0),
    weapon_state_(WeaponStateReady),
    state_counter_(0),
    is_locking_on_(false),
    sighting_ms_(0),
    energy_(1.0),
    hp_(1.0)
{
    TheDatabase::instance().create(id_, "robo");
    model_ = TheDatabase::instance().find(id_);
    delta_next_position_.copy_from(*(model_->position()));
}

Robo::~Robo()
{
    model_ = 0; // will be deleted by the database
    SAFE_DELETE(view_);
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

void Robo::view(int width, int height, double near_clip, double far_clip)
{
    ASSERT(!view_);
    view_ = new View(width, height, near_clip, far_clip);
}

View* Robo::view() const { return view_; }

double Robo::energy() const { return energy_; }

double Robo::hp() const { return hp_; }

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

void Robo::absorb_energy()
{
    unsigned delta = TheTime::instance().delta();
    energy_ = energy_ + AbsorptionEnergyPerMs * delta;

    if (energy_ >= 1.0)
    {
        energy_ = 1.0;
    }
}

void Robo::boost(const Vector3& direction)
{
    if (energy_ <= 0.0)
    {
        absorb_energy();
        return;
    }

    unsigned delta = TheTime::instance().delta();
    energy_ = energy_ - BoostEnergyPerMs * delta;

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
    model_->draw(   view.get_perspective_matrix(),
                    TheEnvironment::Brightness,
                    TheEnvironment::AmbientBrightness,
                    TheEnvironment::LightVector);
}

namespace
{

double normalize_angle(double angle)
{
    if (angle > 180.0)
    {
        angle = angle - 360.0;
    }
    else if (angle < -180.0)
    {
        angle = angle + 360.0;
    }
    return angle;
}

void normalize_angle(Vector3* angle)
{
    angle->x = normalize_angle(angle->x);
    angle->y = normalize_angle(angle->y);
    angle->z = normalize_angle(angle->z);
}

} // namespace -

void Robo::fire_bullet(const Robo* opponent)
{
    if (weapon_state_ != WeaponStateReady)
    {
        return;
    }

    const Vector3& angle = *view_->angle();

    Vector3 modified_angle(angle.x, angle_zx_, 0.0);
    normalize_angle(&modified_angle);
    Ai::TheArmoury::instance().fire(    *this,
                                        *model_->position(),
                                        modified_angle,
                                        opponent,
                                        is_locking_on_);

    weapon_state_ = WeaponStateCharging;
}

namespace
{

// sighting
const double MaxDepthCanLockOn      = 100.0;
const double MinDepthCanLockOn      = 3.0;
const double HalfThetaAtMaxDepth    = 1.0;
const double HalfHeightAtMaxDepth
= MaxDepthCanLockOn * GameLib::tan(HalfThetaAtMaxDepth);
const double HalfThetaAtMinDepth
= GameLib::atan2(HalfHeightAtMaxDepth, MinDepthCanLockOn);
const double HalfHeightAtMinDepth
= MinDepthCanLockOn * GameLib::tan(HalfThetaAtMinDepth);
const unsigned MsToCompleteLockOn   = 3000;

double calc_half_theta_at_depth(double depth)
{
    return GameLib::atan2(HalfHeightAtMaxDepth, depth);
}

} // namespace -

double Robo::get_half_sight_size_at_depth(const Robo& opponent) const
{
    Vector3 to_opponent_point(*opponent.center());
    to_opponent_point.subtract(*center());
    const double depth = to_opponent_point.length();
    const double theta = calc_half_theta_at_depth(depth);
    const double length_at_depth = depth * GameLib::tan(theta);
    return length_at_depth;
}

double Robo::get_lock_on_rate() const
{
    double rate = static_cast< double >(sighting_ms_) / MsToCompleteLockOn;
    return rate > 1.0 ? 1.0 : rate < 0.0 ? 0.0: rate;
}

double Robo::get_sight_depth(const Robo& opponent) const
{
    Vector3 to_opponent_point(*opponent.center());
    to_opponent_point.subtract(*view_->center());
    const double depth = to_opponent_point.length();
    return depth;
}

void Robo::get_triangles(std::vector< Triangle >* triangles) const
{
    size_t indexes_size = model_->indexes_size();
    const int* indexes = model_->indexes();
    triangles->reserve(indexes_size / 3);
    const Vector3* vertexes = model_->vertexes();

    for (size_t i = 0; i < indexes_size; i = i + 3)
    {
        Triangle triangle(  vertexes[indexes[i]],
                            vertexes[indexes[i + 1]],
                            vertexes[indexes[i + 2]]);
        triangle.p0.add(*center());
        triangle.p1.add(*center());
        triangle.p2.add(*center());
        triangles->push_back(triangle);
    }
}

Cuboid Robo::locus_cuboid() const
{
    const Vector3* vertexes = model_->vertexes();
    size_t size = model_->vertexes_size();
    double max_length = 0.0;

    for (size_t i = 0; i < size; ++i)
    {
        if (vertexes[i].length() > max_length)
        {
            max_length = vertexes[i].length();
        }
    }

    Vector3 half_size(max_length, max_length, max_length);

    Vector3 previous(*center());
    previous.subtract(delta_next_position_);

    Vector3 balance(*center());
    balance.subtract(previous);
    balance.divide(2.0);

    half_size.x = half_size.x + balance.x;
    half_size.y = half_size.y + balance.y;
    half_size.z = half_size.z + balance.z;

    balance.add(previous);

    return Cuboid(balance, half_size);
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

    if (is_locking_on_)
    {
        *oss << "is locking on!";
    }
    else
    {
        *oss << "NOT locking on...";
        *oss << sighting_ms_;
    }
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
    y0.y = y0.y - 1.0;
    Vector3 y1(balance);
    y1.y = y1.y + 1.0;
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

void Robo::update(const Robo& opponent)
{
    lock_on(opponent);
    set_delta_next_position();
    charge_weapon();
    view_->follow(*this);
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

void Robo::was_shot(double damage)
{
    hp_ = hp_ - damage;

    if (hp_ - damage < 0.0)
    {
        hp_ = 0.0;
    }
}

namespace
{

bool is_sighting(   const Vector3& self_point,
                    const Vector3& opponent_point,
                    const Vector3& angle)
{
    Vector3 to_opponent(opponent_point);
    to_opponent.subtract(self_point);
    const double depth = to_opponent.length();

    if (depth < MinDepthCanLockOn || depth > MaxDepthCanLockOn)
    {
        return false;
    }

    const double abs_theta = std::abs(calc_half_theta_at_depth(depth));

    Matrix44 rotation;
    rotation.rotate_yz(-angle.x);
    rotation.rotate_zx(-angle.y);
    rotation.multiply(&to_opponent);

    // zero at (0, 0, -1)
    const double theta_zx = normalize_angle(GameLib::atan2( to_opponent.x,
                                                            to_opponent.z)
                                            - 180.0);
    const double theta_yz = normalize_angle(GameLib::atan2( to_opponent.z,
                                                            to_opponent.y)
                                            + 90.0);

    return std::abs(theta_zx) < abs_theta && std::abs(theta_yz) < abs_theta;
}

} // namespace -

void Robo::lock_on(const Robo& opponent)
{
    if (!is_sighting(*center(), *opponent.center(), -*view_->angle()))
    {
        is_locking_on_ = false;
        sighting_ms_ = 0;
        return;
    }

    if (is_locking_on_)
    {
        return;
    }

    if (sighting_ms_ > MsToCompleteLockOn)
    {
        is_locking_on_ = true;
    }

    unsigned delta = TheTime::instance().delta();
    sighting_ms_ = sighting_ms_ + delta;
}

void Robo::charge_weapon()
{
    if (weapon_state_ == WeaponStateReady)
    {
        state_counter_ = 0;
        return;
    }

    unsigned delta = TheTime::instance().delta();
    state_counter_ = state_counter_ + delta;

    if (state_counter_ > ChargingMs)
    {
        weapon_state_ = WeaponStateReady;
        state_counter_ = 0;
    }
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

    force_.set(0.0, -TheEnvironment::gravity_acceleration() * mass_, 0.0);
}
