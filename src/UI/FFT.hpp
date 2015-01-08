//  UI/FFT.hpp :: FFT visualizer
//  Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef UI_FFT_H
#define UI_FFT_H

#include "../__zzCore.hpp"
#include "../libawe/Define.hpp"

#include "../kiss_fft130/kiss_fft.h"
#include "../kiss_fft130/kiss_fftr.h"

#include <mutex>

namespace UI {

class FFT : public clan::View
{
public:
    enum class IEWindowType : uint8_t
    {
        HANNING     = 'H',
        LANCZOS     = 'L',
        RECTANGULAR = 'R',
        TRIANGULAR  = 'T'
    };

    enum class IEScaleType : uint8_t
    {
        LINEAR = 'L',
        DBFS   = 'D',
        CUBIC  = 'C'
    };

    static std::vector<float> generate_window(IEWindowType const &window, ulong const &size);

private:
    std::mutex      mMutex;     //! Master mutex

protected:
    ////    Class behaviour    /////////////////////////////
    ulong   mFrames;    //! number of frames to process
    ulong   mBands;     //! number of bands to show
    float   mFade;      //! decay strength
    float   mDecay;     //! decay over time

    IEWindowType    mWindowType;
    IEScaleType     mScaleType;

    float   mScale; // range of values to display (for log scale from 0db to x)
    float   mRange; // number of pixels


    ////    Class state    /////////////////////////////////
    kiss_fftr_cfg    kCl,  kCr; // KISSFFT config
    kiss_fft_scalar *kIl, *kIr; // input
    kiss_fft_cpx    *kOl, *kOr; // output

    std::vector<float>  mBandX;         // Plot scale conversion table
    std::vector<float>  mWindow;        // FFT window function coefficients
    float               mWindowScale;   // FFT window magnitude adjustment
    awe::AfBuffer       mOutput;        // Squares of the real and imaginary parts of the FFT output
    awe::AfBuffer       mSpectrum;      // Output spectrum values adjusted to scale for drawing

    awe::AfBuffer       mPrevious;      // Half-buffer of previous run.
    awe::AfBuffer       mVector;        // Spectra direction buffer.

    void setBands (ulong);
    void setWindow(IEWindowType);

public:
    FFT(recti area,
        ulong frames,
        ulong sample_rate,
        ulong bands,
        float fade = 0.1f,
        IEWindowType window = IEWindowType::HANNING,
        IEScaleType  scale  = IEScaleType::DBFS
       );
    ~FFT();

    inline std::mutex &getMutex() { return mMutex; }

    void setSampleRate(ulong rate);

    void calc_FFT(awe::AfBuffer const &buffer);
    void update  (awe::AfBuffer const &buffer);
    void update_2(awe::AfBuffer const &buffer); // Double-buffered update.

    ////    clan::View methods    ////////////////////////////////
    void render_content(clan::Canvas &canvas) override;
};

}

#endif
