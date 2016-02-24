#ifndef ROBOF__THE_HORIZON_H_
#define ROBOF__THE_HORIZON_H_

class Cuboid;
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
};

#endif
