//  Sources/Track.cpp :: Sound mixing track
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#include "Track.hpp"

namespace awe {
namespace Source {

void Track::fpull(Asource* src)
{
    if (src->is_active() == true)
        src->render(mPbuffer, mPconfig);
}

void Track::fpull()
{
    for(Asource* src: mPsources)
        fpull(src);
}

void Track::fflip()
{
    auto s = mObuffer.size();
    mObuffer.clear();
    mObuffer.resize(s);
    mObuffer.swap(mPbuffer);
}

void Track::ffilter()
{
    mOfilter.filter_buffer(mObuffer);
}


Track::Track(size_t sample_rate, size_t frames, std::string name)
    : mName   (name)
    , mPconfig(sample_rate, frames)
    , mPbuffer(2 * frames, 0.f)
    , mObuffer(2 * frames, 0.f)
    , mqActive(true)
{ }

void Track::render(AfBuffer &targetBuffer, const ArenderConfig &targetConfig)
{
    if (targetConfig.quality == ArenderConfig::Quality::SKIP)
        return;

    std::lock(mPmutex, mOmutex);

    size_t a = 0, p = targetConfig.frameOffset;
    size_t const  q = targetConfig.frameOffset + mPconfig.frameCount;

    MutexLockGuard o_lock(mOmutex, std::adopt_lock);
    {
        // Unlock pool mutex immediately after mixing.
        MutexLockGuard p_lock(mPmutex, std::adopt_lock);
        fpull();
        fflip();
    }

    ffilter();

    if (targetConfig.quality == ArenderConfig::Quality::MUTE)
        return;

    AfBuffer::const_pointer src = mObuffer.data();
    AfBuffer::      pointer dst = targetBuffer.data();

    while(p < q)
    {
        dst[p*2  ] += src[a*2  ];
        dst[p*2+1] += src[a*2+1];

        p += 1;
        a += 1;
    }
}

}
}
