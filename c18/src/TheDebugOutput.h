#ifndef ROBOF__THE_DEBUG_OUTPUT_H_
#define ROBOF__THE_DEBUG_OUTPUT_H_

class Robo;

class TheDebugOutput
{
private:
    static int row;

public:
    static void clear();
    static void print(const int& a);
    static void print(const Robo& roob);
};

#endif
