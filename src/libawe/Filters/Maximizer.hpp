//  Filters/Maximizer.hpp :: Audio signal maximizer
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_FILTER_MAXIMIZER_H
#define AWE_FILTER_MAXIMIZER_H

#include "../Filter.hpp"

namespace awe {
namespace Filter {

/** A simple hard-limiter that boosts and limits audio signals.
 *
 *  This filter boosts the input audio signal and then passes it through a hard
 *  limiter, which limits the audio signal to the specified loudness threshold.
 *  The limiter release is softened as it falls back under the threshold to
 *  reduce the harsh-sounding effect on plain hard-clipping filters.
 */
template< const Achan Channels >
class Maximizer : public Afilter< Channels >
{
private:
    unsigned    mFrameRate;     //!< Frame rate

    Afloat      mBoost;         //!< Pre-limiter boost
    Afloat      mThreshold;     //!< Limiter threshold
    Afloat      mSlowRelease;   //!< Slow-release time in milliseconds
    Afloat      mPeakRelease;   //!< Peak-release time in milliseconds
    Afloat      mCeiling;       //!< Post-limiter ceiling

    Afloat      mDecayRate;     //!< Current release rate
    Afloat      mGain;          //!< Current limiter level

    ////    Metering attributes    ////
    Afloat      mPeakSample;    //<! Peak sample on last update

private:
    /** Decay rate calculator.
     *
     *  Calculates the small difference from `a` to `b` for the time period `t`
     *  from the data sampled at a sampling rate of `f`.
     */
    static inline float calc_diff(float a, float b, float t, float f) {
        return (a - b) / (t * f);
    }

public:
    //! Default constructor.
    Maximizer(
        unsigned frame_rate,
        Afloat boost        = awe::from_dBFS( 0.0f),
        Afloat threshold    = awe::from_dBFS(-0.1f),
        Afloat slow_release = 1.0f,
        Afloat peak_release = 200.0f,
        Afloat ceiling      = awe::from_dBFS( 0.0f)
    )   : mFrameRate    (frame_rate)
        , mBoost        (boost)
        , mThreshold    (threshold)
        , mSlowRelease  (slow_release)
        , mPeakRelease  (peak_release)
        , mCeiling      (ceiling)
        , mDecayRate    (0.0f)
        , mGain         (1.0f)
        , mPeakSample   (0.0f)
    { }

    //! Resets the limiter state in the maximizer.
    inline void reset_state() override {
        mDecayRate  = 0.0f;
        mGain       = 1.0f;
        mPeakSample = 0.0f;
    }

    inline void setBoost        (Afloat const &value) { mBoost       = value; }
    inline void setThreshold    (Afloat const &value) { mThreshold   = value; reset_state(); }
    inline void setSlowRelease  (Afloat const &value) { mSlowRelease = value; reset_state(); }
    inline void setPeakRelease  (Afloat const &value) { mPeakRelease = value; reset_state(); }
    inline void setCeiling      (Afloat const &value) { mCeiling     = value; }

    inline Afloat getBoost      () const { return mBoost; }
    inline Afloat getThreshold  () const { return mThreshold; }
    inline Afloat getSlowRelease() const { return mSlowRelease; }
    inline Afloat getPeakRelease() const { return mPeakRelease; }
    inline Afloat getCeiling    () const { return mCeiling; }

    inline Afloat getCurrentGain() const { return mGain; }
    inline Afloat getPeakSample () const { return mPeakSample; }

    /** Performs maximization on the audio buffer.
     *  \param buffer The audio buffer to filter.
     */
    void filter_buffer(AfBuffer &buffer) override
    {
        mPeakSample = 0.0f;

        Afloat* frame = buffer.data();

        for(size_t i = 0; i < buffer.size(); i += Channels, frame += Channels)
        {
            Afloat  framePeak = 0.0f;

            //  Get peak value in frame.
            for(Achan c = 0; c < Channels; c += 1) {
                frame[c] *= mBoost;
                framePeak = std::max(framePeak, std::abs(frame[c]));
            }

            //  Get peak value on this filter run.
            mPeakSample = std::max(mPeakSample, framePeak);

            //  Current peak is over threshold.
            if (framePeak > mThreshold) {
                Afloat newGain = framePeak / mThreshold;

                //  Reset decay rate if higher limiting.
                if (newGain > mGain) {
                    mGain = newGain;
                    mDecayRate = ( (framePeak > 1.0f)
                            ? calc_diff(framePeak,       1.0f, mPeakRelease / 1000.0f, mFrameRate)
                            : calc_diff(framePeak, mThreshold, mSlowRelease / 1000.0f, mFrameRate)
                            );
                }
            }

            //  Apply limiter.
            for(Achan c = 0; c < Channels; c += 1)
                frame[c] *= mCeiling / mGain;

            //  Apply limiter release.
            if (mGain > 1.0f / mThreshold)
            {   //  Limiter level is above peak limit.
                mGain = mGain - mDecayRate;

                //  Switch to slow release if limiter level has gone below peak limit.
                if (mGain < 1.0f / mThreshold)
                    mDecayRate = calc_diff(1.0f, mThreshold, mSlowRelease / 1000.0f, mFrameRate);

            } else if (mGain > 1.0f) {
                //  Limiter level is above threshold.
                mGain = mGain - mDecayRate;
            } else {
                //  Limiter level is below threshold.
                mGain = 1.0f;
            }
        }
    }
};

}
}
#endif
