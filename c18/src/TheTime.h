#ifndef ROBOF_THE_TIME_H_
#define ROBOF_THE_TIME_H_

class TheTime
{
public:
    static void create();
    static void destroy();
    static TheTime instance();

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
