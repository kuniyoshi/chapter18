#ifndef ROBOF__THE_HORIZON_H_
#define ROBOF__THE_HORIZON_H_
#include <vector>

class Cuboid;
class Sphere;
class Triangle;
class View;

class TheHorizon
{
public:
    static void create();
    static void destroy();
    static TheHorizon instance();
    static bool did_create();

private:
    TheHorizon();

public:
    ~TheHorizon();
    Cuboid cuboid() const;
    void draw(const View& view);
    Sphere sphere() const;
    const std::vector< Triangle >* triangles() const;
};

#endif
