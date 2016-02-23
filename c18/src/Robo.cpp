#include "Robo.h"
#include <sstream>
#include "GameLib/Framework.h"
#include "GameLib/Math.h"
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Vector3.h"
#include "TheTime.h"
#include "View.h"

using GraphicsDatabase::Matrix44;
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

const int TotalVertexes                 = 8;
const int VertexesPerTriangle           = 3;
const int VertexIndexSize               = Robo::FaceIdSize * 2 * VertexesPerTriangle;
const int VertexIndex[VertexIndexSize]  = { 2, 3, 6, 3, 7, 6, // Front
                                            0, 2, 4, 2, 6, 4, // Right
                                            1, 0, 5, 0, 4, 5, // Rear
                                            3, 1, 7, 1, 5, 7, // Left
                                            0, 1, 2, 1, 3, 2, // Top
                                            5, 4, 7, 4, 6, 7}; // Bottom

const int VertexesPerRectangle = 4;
const Vector3 UvVertexes[Robo::FaceIdSize * VertexesPerRectangle]
= { Vector3(0.0, 0.0), // Front[0, 3]
    Vector3(0.25, 0.0),
    Vector3(0.0, 0.5),
    Vector3(0.25, 0.5),
    Vector3(0.75, 0.0), // Right[4, 7]
    Vector3(1.0, 0.0),
    Vector3(0.75, 0.5),
    Vector3(1.0, 0.5),
    Vector3(0.25, 0.0), // Rear[8, 11]
    Vector3(0.5, 0.0),
    Vector3(0.25, 0.5),
    Vector3(0.5, 0.5),
    Vector3(0.5, 0.0), // Left[12, 15]
    Vector3(0.75, 0.0),
    Vector3(0.5, 0.5),
    Vector3(0.75, 0.5),
    Vector3(0.0, 0.5), // Top[16, 19]
    Vector3(0.25, 0.5),
    Vector3(0.0, 0.75),
    Vector3(0.25, 0.75),
    Vector3(0.25, 0.5), // Bottom[20, 23]
    Vector3(0.5, 0.5),
    Vector3(0.25, 0.75),
    Vector3(0.5, 0.75)};
const int UvVertexIndex[VertexIndexSize] = {    0, 1, 2, 1, 3, 2, // Front
                                                4, 5, 6, 5, 7, 6, // Right
                                                8, 9, 10, 9, 11, 10, // Rear
                                                12, 13, 14, 13, 15, 14, // Left
                                                16, 17, 18, 17, 19, 18, // Top
                                                20, 21, 22, 21, 23, 22}; // Bottom

const int AngleScale = 3;

} // namespae -

// Vertex Index
//
//   /---> x
//  v
// z
//    0___________________1
//   /                    /
//  /                    /|
// /2__________________3/ |
// |                   |  |
// |                   |  |
// |                   |  |
// |  4                | 5/
// |                   | /
// 6___________________7/

Robo::Robo()
:   texture_(0),
    world_(),
    center_(0.0, Height / 2.0, 0.0),
    vertexes_(),
    force_(),
    velocity_(),
    angle_zx_(0.0),
    mass_(TheMass)
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.createTexture(&texture_, "data/image/robo.tga");

    vertexes_[0].set(-HalfWidth, Height, -HalfWidth);
    vertexes_[1].set(+HalfWidth, Height, -HalfWidth);
    vertexes_[2].set(-HalfWidth, Height, +HalfWidth);
    vertexes_[3].set(+HalfWidth, Height, +HalfWidth);
    vertexes_[4].set(-HalfWidth, 0.0, -HalfWidth);
    vertexes_[5].set(+HalfWidth, 0.0, -HalfWidth);
    vertexes_[6].set(-HalfWidth, 0.0, +HalfWidth);
    vertexes_[7].set(+HalfWidth, 0.0, +HalfWidth);

    center_.add(Vector3(0.0, 0.0, -0.6 - HalfWidth));
}

Robo::~Robo()
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.destroyTexture(&texture_);
}

double Robo::angle_zx() const { return angle_zx_; }

namespace
{

void draw_debug(GameLib::Framework f, const Vector3& value, int row)
{
    std::ostringstream oss;
    oss << "{";
    oss << value.x;
    oss << ", ";
    oss << value.y;
    oss << ", ";
    oss << value.z;
    oss << "}";
    f.drawDebugString(0, row, oss.str().c_str());
}

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

    if ((center_.y > Height / 2.0) && tuned_direction.length() > 0.0)
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

const Vector3& Robo::center() const { return center_; }

void Robo::draw(const View& view) const
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.setTexture(texture_);

    Matrix44 transform(world_);
    transform.dot(view.get_perspective_matrix());

    Vector3 vertexes[TotalVertexes];

    for (int i = 0; i < TotalVertexes; ++i)
    {
        vertexes[i] = vertexes_[i];
        transform.multiply(&vertexes[i]);
    }

    for (int i = 0; i < VertexIndexSize; i = i + VertexesPerTriangle)
    {
        // if (i == 0)
        // {
        //     view.draw_debug_string(&vertexes[VertexIndex[i]].x, 1 + i / VertexesPerTriangle);
        // }

        f.drawTriangle3DH(  &vertexes[VertexIndex[i]].x,
                            &vertexes[VertexIndex[i + 1]].x,
                            &vertexes[VertexIndex[i + 2]].x,
                            &UvVertexes[UvVertexIndex[i]].x,
                            &UvVertexes[UvVertexIndex[i + 1]].x,
                            &UvVertexes[UvVertexIndex[i + 2]].x);
    }
}

void Robo::rotate_zx(int angle_zx)
{
    angle_zx_ = angle_zx_ + static_cast< double >(angle_zx) / AngleScale;
}

void Robo::run(const Vector3& direction)
{
    const double RunAcceleration = 22.0;
    const double a = RunAcceleration * mass_;
    add_force(&force_, angle_zx_, direction, a);
    // center_.add(dir);
}

void Robo::tick()
{
    TheTime t = TheTime::instance();
    double dt = static_cast< double >(t.delta()) / 1000.0;

    stop_if_too_small(&velocity_, force_);

    if ((force_.y < 0.0 || velocity_.y < 0.0) && center_.y <= Height / 2.0)
    {
        force_.y = 0.0;
        velocity_.y = 0.0;
    }

    Vector3 drag(get_drag(velocity_));
    Vector3 friction;

    if (velocity_.y == 0.0 && center_.y <= Height / 2.0)
    {
        // Si: 0.58, Fe: 0.52
        const double FrictionCoef = (0.58 + 0.52) / 2.0;

        friction.x = get_reverse_direction(velocity_.x);
        friction.z = get_reverse_direction(velocity_.z);
        friction.multiply(mass_ * FrictionCoef);
    }

    // GameLib::Framework f = GameLib::Framework::instance();
    // draw_debug(f, velocity_, 1);
    // draw_debug(f, force_, 2);
    // draw_debug(f, drag, 3);
    // draw_debug(f, friction, 4);

    Vector3 dv(force_);
    dv.subtract(drag);
    dv.add(friction);
    dv.multiply(dt);
    dv.divide(mass_);
    velocity_.add(dv);

    Vector3 delta(velocity_);
    delta.multiply(dt);

    center_.add(delta);

    if (center_.y < Height / 2.0)
    {
        center_.y = Height / 2.0;
    }

    force_.set(0.0, -GravityAcceleration * mass_, 0.0);

    world_ = Matrix44();
    world_.rotate_zx(angle_zx_);
    world_.translate(center_);
}

void Robo::warp(const Vector3& diff)
{
    center_.add(diff);
    world_ = Matrix44();
    world_.translate(center_);
}
