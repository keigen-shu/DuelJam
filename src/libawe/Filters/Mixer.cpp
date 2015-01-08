//  Filters/Mixer.cpp :: Basic mixer filter
//  Copyright 2013 Chu Chin Kuan <keigen.shu@gmail.com>

#include "Mixer.hpp"

namespace awe {
namespace Filter {

Asfloatf xLinear(Afloat const &vol, Afloat const &pan)
{
    const Afloat l = (1.0f + pan) / 2.0f;
    Asfloatf f;
    f[0] = 2.0f * l * vol;
    f[1] = 2.0f * (1.0f - l) * vol;
    return f;
}

Asfloatf xSinCos(Afloat const &vol, Afloat const &pan)
{
    const Afloat p = M_PI * (1.0f + pan) / 4.0f;
    Asfloatf f;
    f[0] = (float)cos(p) * vol;
    f[1] = (float)sin(p) * vol;
    return f;
}

}
}
