#ifndef ROBOF__ROBO_H_
#define ROBOF__ROBO_H_
#include <sstream>
#include <string>
#include "GraphicsDatabase/Vector3.h"

namespace GameLib { class Texture; }
namespace GraphicsDatabase { class Model; }
class Cuboid;
class Sphere;
class View;

using GraphicsDatabase::Model;
using GraphicsDatabase::Vector3;

class Robo
{
private:
    Model* model_;
    Vector3 force_;
    Vector3 velocity_;
    Vector3 delta_next_position_;
    double angle_zx_;
    double mass_;

public:
    Robo(const std::string& id);
    ~Robo();
    double angle_zx() const;
    const Vector3* velocity() const;
    void velocity(const Vector3& new_value);
    const Vector3* delta_next_position() const;
    void delta_next_position(const Vector3& new_value);
    void boost(const Vector3& direction);
    const Vector3* center() const;
    Cuboid cuboid() const;
    void commit_next_position();
    void draw(const View& view) const;
    void print(std::ostringstream* oss) const;
    void rotate_zx(int angle_zx);
    void run(const Vector3& direction);
    void set_delta_next_position();
    Sphere sphere() const;
    void warp(const Vector3& to);
};

#endif
