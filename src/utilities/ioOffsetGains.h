#ifndef IO_OFFSET_GAINS_H
#define IO_OFFSET_GAINS_H

class ioOffsetGains
{
public:
    ioOffsetGains();
    void setOffset(float offset);
    float getOffset();
    void setGain(float gain);
    float getGain();
    ~ioOffsetGains();

private:
    float offset_p;
    float gain_p;
};

#endif