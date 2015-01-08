//  Filters/Metering.cpp :: Audio buffer metrics collector
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#include "Metering.hpp"

namespace awe {
namespace Filter {

AscMetering::AscMetering(Afloat freq, Afloat decay)
    : mFreq (freq)
    , mDecay(decay)
    , mPeak ({0.0f, 0.0f})
    , mRMS  ({0.0f, 0.0f})
    , mdOCI ({int16_t{0}, int16_t{0}})
    , mdRMS ({0.0f, 0.0f})
{

}

void AscMetering::filter_buffer(AfBuffer &buffer)
{
    mPeak *= 0;
    mRMS  *= 0;

    Asfloatf mSum({0.0f, 0.0f});

    for(size_t i = 0; i < buffer.size() / 2; i++)
    {
        Asfloatf m = Asfloatf::from_buffer(buffer, i);
        m.abs();

        mPeak[0] = std::max(mPeak[0], m[0]);
        mPeak[1] = std::max(mPeak[1], m[1]);

        mSum[0] += m[0] * m[0];
        mSum[1] += m[1] * m[1];
    }

    mSum /= buffer.size() / 2;

    mRMS[0] = sqrt(mSum[0]);
    mRMS[1] = sqrt(mSum[1]);

    mdRMS = mdRMS * mdRMS + mRMS * mRMS;
    mdRMS /= 2.0f;

    mdRMS[0] = sqrt(mdRMS[0]);
    mdRMS[1] = sqrt(mdRMS[1]);

    mdOCI[0] = ( mPeak[0] > 1.0f  ) ? 105 :
               ( mdRMS[0] > 0.25f ) ? std::max(int16_t{60}, mdOCI[0]) : // ~ -24dB RMS
                 mdOCI[0];
    mdOCI[0] = ( mdOCI[0] > 0.25f ) ? mdOCI[0] - 1 : 0;

    mdOCI[1] = ( mPeak[1] > 1.0f  ) ? 105 :
               ( mdRMS[1] > 0.25f ) ? std::max(int16_t{60}, mdOCI[1]) : // ~ -24dB RMS
                 mdOCI[1];
    mdOCI[1] = ( mdOCI[1] > 0.25f ) ? mdOCI[1] - 1 : 0;
}

}
}
