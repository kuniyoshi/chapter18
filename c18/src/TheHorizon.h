#ifndef ROBOF_THE_HORIZON_H_
#define ROBOF_THE_HORIZON_H_
#include "GraphicsDatabase/Vector3.h"

using GraphicsDatabase::Vector3;

namespace GameLib { class Texture; }
class View;

class TheHorizon
{
private:
    GameLib::Texture* texture_;
    double height_;
    Vector3 uv_vertexes_[4];
    Vector3 vertexes_[4];

public:
    TheHorizon();
    ~TheHorizon();
    void draw(const View& view);
};

#endif
