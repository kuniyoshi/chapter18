#include "Ai/TheArmoury.h"
#include <cassert>
#include "GraphicsDatabase/Vector3.h"
#include "Bullet.h"
#include "Cuboid.h"
#include "Robo.h"
#include "TheCollision.h"
#include "TheDatabase.h"
#include "TheHorizon.h"
#include "Triangle.h"
#include "View.h"

using GraphicsDatabase::Vector3;

namespace Ai
{

namespace
{

const int MaxBulletPerRobo = 1000;

class Impl
{
private:
    Bullet* bullets_;

public:
    Impl();
    ~Impl();
    void draw(const View& view) const;
    bool fire(  const Robo& robo,
                const Vector3& from,
                const Vector3& direction,
                const Robo* opponent,
                const bool is_locking_on);
    void make_collision(TheHorizon horizon);
    void make_collision(Robo* target);
    void update();
};

Impl::Impl()
{
    bullets_ = new Bullet[2 * MaxBulletPerRobo];
    TheDatabase::instance().create_model("bullet", "bullet");
}

Impl::~Impl()
{
    delete[] bullets_;
    bullets_ = 0;
}

void Impl::draw(const View& view) const
{
    for (int i = 0; i < 2 * MaxBulletPerRobo; ++i)
    {
        bullets_[i].draw(view);
    }
}

namespace
{

int count_bullet(int id, const Bullet* bullets)
{
    int count = 0;

    for (int i = 0; i < 2 * MaxBulletPerRobo; ++i)
    {
        if (bullets[i].is_owned_by(id))
        {
            ++count;
        }
    }

    return count;
}

} // namespace -

bool Impl::fire(    const Robo& robo,
                    const Vector3& from,
                    const Vector3& angle,
                    const Robo* opponent,
                    const bool is_locking_on)
{
    int count = count_bullet(robo.int_id(), bullets_);

    if (count >= MaxBulletPerRobo)
    {
        return false;
    }

    bool did_fire = false;

    for (int i = 0; i < 2 * MaxBulletPerRobo; ++i)
    {
        if (bullets_[i].is_owned())
        {
            continue;
        }

        bullets_[i].initialize( robo.int_id(),
                                from,
                                angle,
                                &robo,
                                opponent,
                                is_locking_on);
        did_fire = true;
        break;
    }

    return did_fire;
}

void Impl::make_collision(TheHorizon horizon)
{
    for (int i = 0; i < 2 * MaxBulletPerRobo; ++i)
    {
        if (!bullets_[i].is_owned())
        {
            continue;
        }

        if (bullets_[i].did_collide())
        {
            continue;
        }

        TheCollision::burn(&bullets_[i], horizon);
    }
}

void Impl::make_collision(Robo* target)
{
    std::vector< Triangle > triangles;
    target->get_triangles(&triangles);
    Cuboid cuboid = target->locus_cuboid();

    for (int i = 0; i < 2 * MaxBulletPerRobo; ++i)
    {
        if (!bullets_[i].is_owned())
        {
            continue;
        }

        if (bullets_[i].did_collide())
        {
            continue;
        }

        if (bullets_[i].is_owned_by(target->int_id()))
        {
            continue;
        }

        TheCollision::burn(&bullets_[i], target, cuboid, triangles);
    }
}

void Impl::update()
{
    for (int i = 0; i < 2 * MaxBulletPerRobo; ++i)
    {
        bullets_[i].update();
    }
}

Impl* g_impl = 0;

} // namespace -

void TheArmoury::create()
{
    assert(!g_impl);
    g_impl = new Impl();
}

void TheArmoury::destroy()
{
    assert(g_impl);
    delete g_impl;
    g_impl = 0;
}

TheArmoury TheArmoury::instance()
{
    return TheArmoury();
}

bool TheArmoury::did_create()
{
    return !!g_impl;
}

TheArmoury::TheArmoury() {}

TheArmoury::~TheArmoury() {}

void TheArmoury::draw(const View& view) const { g_impl->draw(view); }

bool TheArmoury::fire(  const Robo& robo,
                        const Vector3& from,
                        const Vector3& direction,
                        const Robo* opponent,
                        const bool is_locking_on) const
{
    return g_impl->fire(robo, from, direction, opponent, is_locking_on);
}

template< class T >
void TheArmoury::make_collision(T to_what) const
{
    g_impl->make_collision(to_what);
}

template void TheArmoury::make_collision(TheHorizon) const;
template void TheArmoury::make_collision(Robo*) const;

void TheArmoury::update() const { g_impl->update(); }

} // namespace Ai
