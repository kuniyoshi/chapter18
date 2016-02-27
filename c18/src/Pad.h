#ifndef ROBOF__PAD_H_
#define ROBOF__PAD_H_

class Pad
{
public:
    enum Input
    {
        LeftStickUp,
        LeftStickDown,
        LeftStickLeft,
        LeftStickRight,
        LeftTrigger,
        RightTrigger,
        RightStickUp,
        RightStickDown,
        RightStickLeft,
        RightStickRight,
        A,
        B,
        Option,
        Reset,
        Terminate,
    };

private:
    int id_;

public:
    Pad(int id);
    ~Pad();
    bool isOn(Input input) const;
    bool isTriggered(Input input) const;
};

#endif