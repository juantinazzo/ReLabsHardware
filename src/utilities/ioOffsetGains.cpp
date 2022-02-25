#include "ioOffsetGains.h"

ioOffsetGains::ioOffsetGains()
{
}

ioOffsetGains::~ioOffsetGains()
{
}

float ioOffsetGains::getGain()
{
    return gain_p;
}

float ioOffsetGains::getOffset()
{
    return offset_p;
}

void ioOffsetGains::setGain(float gain)
{
    gain_p = gain;
}

void ioOffsetGains::setOffset(float offset)
{
    offset_p = offset;
}
