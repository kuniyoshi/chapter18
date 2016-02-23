#ifndef ROBOF_THE_HORIZON_H_
#define ROBOF_THE_HORIZON_H_

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
    void draw(const View& view);
};

#endif
