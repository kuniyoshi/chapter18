#ifndef ROBOFPAD_H_
#define ROBOFPAD_H_

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
        Option2,
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
