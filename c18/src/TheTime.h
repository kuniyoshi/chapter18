#ifndef ROBOF__THE_TIME_H_
#define ROBOF__THE_TIME_H_

class TheTime
{
public:
    static void create();
    static void destroy();
    static TheTime instance();
    static bool did_create();

private:
    TheTime();

public:
    ~TheTime();

public:
    double rate() const;
    void rate(double new_value) const;
    unsigned delta() const;
    void tick() const;
};

#endif
