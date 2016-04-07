#ifndef ROBOFWALL_H_
#define ROBOFWALL_H_
#include <string>
#include <vector>

namespace GraphicsDatabase { class Model; }
namespace GraphicsDatabase { class Vector3; }
class Triangle;
class View;

using GraphicsDatabase::Model;
using GraphicsDatabase::Vector3;

class Wall
{
private:
    Model* model_;
    std::vector< Triangle > triangles_;

public:
    Wall(const std::string& id);
    ~Wall();
    void draw(const View& view) const;
    const std::vector< Triangle >* triangles() const;
    void warp(const Vector3& position);
};

#endif
