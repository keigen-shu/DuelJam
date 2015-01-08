#include "ChronoTTime.hpp"

bool TTime::operator== (const TTime &other) const
{
    if (t == other.t && b == other.b && m == other.m)
        return true;
    return false;
}

bool TTime::operator>= (const TTime &other) const
{
    if (t >= other.t && (b == other.b && m == other.m))
        return true;
    else if ((b > other.b && m == other.m) || m > other.m)
        return true;
    else
        return false;
}

bool TTime::operator<= (const TTime &other) const
{
    if (t <= other.t && (b == other.b && m == other.m))
        return true;
    else if ((b < other.b && m == other.m) || m < other.m)
        return true;
    else
        return false;
}

bool TTime::operator> (const TTime &other) const
{
    if (t > other.t && (b == other.b && m == other.m))
        return true;
    else if ((b > other.b && m == other.m) || m > other.m)
        return true;
    else
        return false;
}

bool TTime::operator< (const TTime &other) const
{
    if (t < other.t && (b == other.b && m == other.m))
        return true;
    else if ((b < other.b && m == other.m) || m < other.m)
        return true;
    else
        return false;
}


TSignature getTimeSignature(double z)
{
    for(auto p : kTimeSignatureLookup)
        if (p.first == z) return p.second;

    return { 0, 0 };
}

