#include "TheDebugOutput.h"
#include <sstream>
#include "GameLib/Framework.h"
#include "Robo.h"
#include "Triangle.h"
#include "View.h"

int TheDebugOutput::row = 0;

void TheDebugOutput::clear() { TheDebugOutput::row = 0; }

void TheDebugOutput::print(const unsigned& a)
{
    std::ostringstream oss;
    oss << a;

    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, oss.str().c_str());

    ++row;
}

void TheDebugOutput::print(const int& a)
{
    std::ostringstream oss;
    oss << a;

    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, oss.str().c_str());

    ++row;
}

void TheDebugOutput::print(const double& a)
{
    std::ostringstream oss;
    oss << a;

    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, oss.str().c_str());

    ++row;
}

void TheDebugOutput::print(const char* string)
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, string);

    ++row;
}

template< class T >
void TheDebugOutput::print(const T& some)
{
    std::ostringstream oss;
    some.print(&oss);

    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, oss.str().c_str());

    ++row;
}

template void TheDebugOutput::print(const Robo&);
template void TheDebugOutput::print(const Triangle&);
template void TheDebugOutput::print(const View&);
