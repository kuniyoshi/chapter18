#ifndef ROBOF__ROBO_H_
#define ROBOF__ROBO_H_
#include <string>
#include "GraphicsDatabase/Vector3.h"

namespace GameLib { class Texture; }
namespace GraphicsDatabase { class Model; }
class View;

using GraphicsDatabase::Model;
using GraphicsDatabase::Vector3;

class Robo
{
private:
    Model* model_;
    Vector3 force_;
    Vector3 velocity_;
    double angle_zx_;
    double mass_;

public:
    Robo(const std::string& id);
    ~Robo();
    double angle_zx() const;
    void boost(const Vector3& direction);
    const Vector3* center() const;
    void draw(const View& view) const;
    void rotate_zx(int angle_zx);
    void run(const Vector3& direction);
    void tick();
    void warp(const Vector3& to);
};

#endif
