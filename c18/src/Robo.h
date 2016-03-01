#ifndef ROBOF__ROBO_H_
#define ROBOF__ROBO_H_
#include <sstream>
#include <string>
#include <vector>
#include "GraphicsDatabase/Vector3.h"

namespace GameLib { class Texture; }
namespace GraphicsDatabase { class Model; }
class Cuboid;
class Segment;
class Sphere;
class View;

using GraphicsDatabase::Model;
using GraphicsDatabase::Vector3;

class Robo
{
public:
    enum WeaponState
    {
        WeaponStateReady,
        WeaponStateCharging,
    };

private:
    const std::string id_;
    Model* model_;
    Vector3 force_;
    Vector3 velocity_;
    Vector3 delta_next_position_;
    double angle_zx_;
    double mass_;
    WeaponState weapon_state_;
    unsigned state_counter_;

public:
    Robo(const std::string& id);
    ~Robo();
    const std::string& id() const;
    int int_id() const;
    const Vector3* force() const;
    void force(const Vector3& new_value);
    const Vector3* velocity() const;
    void velocity(const Vector3& new_value);
    const Vector3* delta_next_position() const;
    void delta_next_position(const Vector3& new_value);
    double angle_zx() const;
    void boost(const Vector3& direction);
    const Vector3* center() const;
    Cuboid cuboid() const;
    void commit_next_position();
    void draw(const View& view) const;
    void fire_bullet(const Vector3& angle);
    void print(std::ostringstream* oss) const;
    void rotate_zx(int angle_zx);
    void run(const Vector3& direction);
    Segment segment() const;
    std::vector< Segment > segments() const;
    void set_model_angle_zx(double new_value);
    Sphere sphere() const;
    void update();
    void warp(const Vector3& to);

private:
    void charge_weapon();
    void set_delta_next_position();
};

#endif
