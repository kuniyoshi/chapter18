#ifndef ROBOF__AI__THE_ARMOURY_H_
#define ROBOF__AI__THE_ARMOURY_H_

namespace GraphicsDatabase { class Vector3; }
class Robo;
class TheHorizon;
class View;

using GraphicsDatabase::Vector3;

namespace Ai
{

class TheArmoury
{
public:
    static void create();
    static void destroy();
    static TheArmoury instance();
    static bool did_create();

private:
    TheArmoury();

public:
    ~TheArmoury();
    void draw(const View& view) const;
    bool fire(  const Robo& robo,
                const Vector3& from,
                const Vector3& angle,
                const Robo* opponent,
                const bool is_locking_on) const;
    template< class T >
    void make_collision(T to_what) const;
    void update() const;
};

} // namespace Ai

#endif
