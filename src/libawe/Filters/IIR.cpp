//  Filters/IIR.cpp :: Infinite Impulse Response Filter
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#include "IIR.hpp"

namespace awe {
namespace Filter {
namespace IIR {


Coeffs newLPF(const double rate, const double freq) noexcept
{
    Coeffs k;

    const double n = 1.0 / tan(M_PI * freq / rate);
    const double c = 1.0 / (1.0 + sqrt(2.0)*n + n*n);

    k[0] = c;
    k[1] = c * 2.0;
    k[2] = c;
    k[3] = 1.0;
    k[4] = c * 2.0 * (1.0 - n*n);
    k[5] = c *(1.0 - sqrt(2.0)*n + n*n);

    k[0] /= k[3]; k[0] = (k[0] == k[0]) ? k[0] : 0;
    k[1] /= k[3]; k[1] = (k[1] == k[1]) ? k[1] : 0;
    k[2] /= k[3]; k[2] = (k[2] == k[2]) ? k[2] : 0;
    k[4] /= k[3]; k[4] = (k[4] == k[4]) ? k[4] : 0;
    k[5] /= k[3]; k[5] = (k[5] == k[5]) ? k[5] : 0;

    return k;
}

Coeffs newHPF(const double rate, const double freq) noexcept
{
    Coeffs k;

    const double n = tan(M_PI * freq / rate);
    const double c = 1.0 / (1.0 + sqrt(2.0)*n + n*n);

    k[0] = c;
    k[1] = c * -2.0;
    k[2] = c;
    k[3] = 1.0;
    k[4] = c *  2.0 * (n*n - 1.0);
    k[5] = c * (1.0 - sqrt(2.0)*n + n*n);

    k[0] /= k[3]; k[0] = (k[0] == k[0]) ? k[0] : 0;
    k[1] /= k[3]; k[1] = (k[1] == k[1]) ? k[1] : 0;
    k[2] /= k[3]; k[2] = (k[2] == k[2]) ? k[2] : 0;
    k[4] /= k[3]; k[4] = (k[4] == k[4]) ? k[4] : 0;
    k[5] /= k[3]; k[5] = (k[5] == k[5]) ? k[5] : 0;

    return k;
}

void process_one
        ( PartialCoeffs const & b
        , PartialCoeffs const & a
        , DelayLine & z
        , double & x
        ) noexcept
{
    const double  y = x * b[0] + z[0];
    z[0] = x * b[1] - y * a[1] + z[1];
    z[1] = x * b[2] - y * a[2];

    //  Clip very small values ( < -240dB ) to 0
    if (z[0] > -1.0e-12 && z[0] < 1.0e-12)
        z[0] = 0;

    if (z[1] > -1.0e-12 && z[1] < 1.0e-12)
        z[1] = 0;

    x = y;
}


}
}
}
