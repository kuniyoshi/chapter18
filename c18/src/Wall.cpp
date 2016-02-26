#include "Wall.h"
#include <string>
#include <vector>
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Model.h"
#include "GraphicsDatabase/Vector3.h"
#include "TheDatabase.h"
#include "Triangle.h"
#include "View.h"

using GraphicsDatabase::Matrix44;
using GraphicsDatabase::Vector3;

Wall::Wall(const std::string& id)
: model_(0)
{
    TheDatabase::instance().create(id, "wall");
    model_ = TheDatabase::instance().find(id);
}

Wall::~Wall()
{
    model_ = 0; // model_ will be deleted on ~TheDatabase
}

void Wall::draw(const View& view) const
{
    model_->draw(view.get_perspective_matrix());
}

std::vector< Triangle > Wall::triangles() const
{
    const Vector3* vertexes = model_->vertexes();
    size_t vertexes_size = model_->vertexes_size();
    const int* indexes = model_->indexes();
    size_t indexes_size = model_->indexes_size();
    size_t triangles_size = indexes_size / 3;
    std::vector< Triangle > triangles;
    triangles.reserve(triangles_size);

    Vector3 balance(*(model_->position()));

    for (size_t i = 0; i < indexes_size; i = i + 3)
    {
        Triangle triangle(  vertexes[indexes[i]],
                            vertexes[indexes[i + 1]],
                            vertexes[indexes[i + 2]]);
        triangle.p0.add(balance);
        triangle.p1.add(balance);
        triangle.p2.add(balance);
        triangles.push_back(triangle);
    }

    return triangles;
}

void Wall::warp(const Vector3& position)
{
    model_->position(position);
}
