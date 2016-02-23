#ifndef ROBOF_ROBO_H_
#define ROBOF_ROBO_H_
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Vector3.h"

using GraphicsDatabase::Matrix44;
using GraphicsDatabase::Vector3;

namespace GameLib { class Texture; }
class View;

class Robo
{
public:
    enum FaceId
    {
        FaceIdFront,
        FaceIdRight,
        FaceIdRear,
        FaceIdLeft,
        FaceIdTop,
        FaceIdBottom,

        FaceIdSize,
    };

private:
    GameLib::Texture* texture_;
    Matrix44 world_;
    Vector3 center_;
    Vector3 vertexes_[8];
    Vector3 force_;
    Vector3 velocity_;
    double angle_zx_;
    double mass_;

public:
    Robo();
    ~Robo();
    double angle_zx() const;
    void boost(const Vector3& direction);
    const Vector3& center() const;
    void draw(const View& view) const;
    void rotate_zx(int angle_zx);
    void run(const Vector3& direction);
    void tick();
    void warp(const Vector3& diff);
};

#endif
