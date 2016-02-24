#include "TheDebugOutput.h"
#include <sstream>
#include "GameLib/Framework.h"
#include "Robo.h"

int TheDebugOutput::row = 0;

void TheDebugOutput::clear() { TheDebugOutput::row = 0; }

void TheDebugOutput::print(const int& a)
{
    std::ostringstream oss;
    oss << a;

    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, oss.str().c_str());

    ++row;
}

void TheDebugOutput::print(const Robo& robo)
{
    std::ostringstream oss;
    robo.print(&oss);

    GameLib::Framework f = GameLib::Framework::instance();
    f.drawDebugString(0, row, oss.str().c_str());

    ++row;
}
