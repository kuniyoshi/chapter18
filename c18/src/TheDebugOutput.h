#ifndef ROBOF__THE_DEBUG_OUTPUT_H_
#define ROBOF__THE_DEBUG_OUTPUT_H_

class Robo;
class Triangle;

class TheDebugOutput
{
private:
    static int row;

public:
    static void clear();
    static void print(const unsigned& a);
    static void print(const int& a);
    static void print(const double& a);
    static void print(const char* string);
    template< class T >
    static void print(const T& some);
};

#endif
