#include "TheTime.h"
#include "GameLib/Framework.h"

namespace
{

class Impl
{
private:
    double rate_;
    unsigned delta_;
    unsigned now_;
    unsigned previous_;

public:
    Impl();
    double rate() const;
    void rate(double new_value);
    unsigned delta() const;
    void tick();
};

Impl::Impl()
:   rate_(1.0),
    delta_(0), now_(0), previous_(0)
{}

double Impl::rate() const { return rate_; }

void Impl::rate(double new_value) { rate_ = new_value; }

unsigned Impl::delta() const { return delta_; }

void Impl::tick()
{
    GameLib::Framework f = GameLib::Framework::instance();
    previous_ = now_;
    now_ = f.time();
    delta_ = static_cast< unsigned >((now_ - previous_) * rate_);
}

Impl* g_impl = 0;

} // namespace -

TheTime::TheTime() {}

TheTime::~TheTime() {}

TheTime TheTime::instance() { return TheTime(); }

bool TheTime::did_create() { return !!g_impl; }

void TheTime::create()
{
    ASSERT(!g_impl);
    g_impl = new Impl();
}

void TheTime::destroy()
{
    ASSERT(g_impl);
    SAFE_DELETE(g_impl);
}

double TheTime::rate() const { return g_impl->rate(); }

void TheTime::rate(double new_value) const { g_impl->rate(new_value); }

unsigned TheTime::delta() const { return g_impl->delta(); }

void TheTime::tick() const { g_impl->tick(); }
