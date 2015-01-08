#include "FFT.hpp"
#include "../libawe/Frame.hpp"

namespace UI {

inline double _sinc(double const &x) { return sin(M_PI * x) / (M_PI * x); }
static constexpr double _lanczos_size = 3.0f;
inline constexpr double _lanczos_sinc(double const &x)
{
    return
        (x == 0.0)          ? 1.0 : (
        (x > _lanczos_size) ? 0.0 : (
            _sinc(x) * _sinc(x / _lanczos_size)
        )
        );
}

std::vector<float> FFT::generate_window(FFT::IEWindowType const &type, ulong const &size)
{
    std::vector<float> window(size);

    switch (type)
    {
        case FFT::IEWindowType::LANCZOS:
            for(ulong t = 0; t < size; ++t)
                window[t] = _lanczos_sinc(static_cast<double>(2 * t) / static_cast<double>(size - 1) - 1.0f);
            break;

        case FFT::IEWindowType::HANNING:
            for(ulong t = 0; t < size; ++t)
                window[t] = 0.5 * (1.0 - cos( 2.0 * M_PI * static_cast<double>(t) / static_cast<double>(size - 1)));
            break;

        case FFT::IEWindowType::RECTANGULAR:
            for(ulong t = 0; t < size; ++t)
                window[t] = 1.0f;
            break;

        case FFT::IEWindowType::TRIANGULAR:
            for(ulong t = 0; t < size / 2; ++t)
                window[t           ] =       static_cast<double>(2 * t) / static_cast<double>(size - 1),
                window[t + size / 2] = 1.0 - static_cast<double>(2 * t) / static_cast<double>(size - 1);
            break;

    }

    return window;
}


FFT::FFT(
    recti area,
    ulong frames,
    ulong sample_rate,
    ulong bands,
    float fade,
    IEWindowType window,
    IEScaleType  scale
)   : clan::View()
    , mFrames       (frames - (frames % 2))
    , mFade         (fade)
    , mWindowType   (window)
    , mScaleType    (scale)
    , mScale        (1.0f)
    , mRange        (-awe::dBFS_limit / mScale)
    , mOutput       (2, mFrames)
    , mSpectrum     (2, 0)
    , mPrevious     (2, mFrames)
    , mVector       (1, mFrames + 1)
{
	box_style.set_absolute();
	box_style.set_left(area.left);
	box_style.set_top(area.top);
	box_style.set_right(area.right);
	box_style.set_bottom(area.bottom);

    setBands(bands);
    setWindow(window);
    setSampleRate(sample_rate);

    kCl = kiss_fftr_alloc(mFrames, 0, NULL, NULL);
    kCr = kiss_fftr_alloc(mFrames, 0, NULL, NULL);

    kIl = (kiss_fft_scalar*)calloc(mFrames, sizeof(kiss_fft_scalar));
    kIr = (kiss_fft_scalar*)calloc(mFrames, sizeof(kiss_fft_scalar));

    kOl = (kiss_fft_cpx*)calloc(mFrames, sizeof(kiss_fft_cpx));
    kOr = (kiss_fft_cpx*)calloc(mFrames, sizeof(kiss_fft_cpx));

    set_focus_policy(clan::FocusPolicy::reject);
}

FFT::~FFT ()
{
    kiss_fftr_free(kCl);
    kiss_fftr_free(kCr);

    free(kIl);
    free(kIr);
    free(kOl);
    free(kOr);
}

void FFT::setSampleRate(ulong rate)
{
    mDecay =
        mFade * (static_cast<float>(mFrames) / static_cast<float>(rate));
}

void FFT::setBands(ulong n)
{
    mBands = std::min(mFrames, n);

    mBandX.clear();
    mBandX.reserve(1 + mBands);
    mBandX.resize (1 + mBands);

    mSpectrum.clear();
    std::vector<float>(2 * mBands, 0.f).swap(mSpectrum);

    for(ulong i = 0; i <= mBands; ++i)
        mBandX[i] = pow(
            static_cast<float>(mFrames / 2),
            static_cast<float>(i) / static_cast<float>(mBands)
            ) - 0.5f;
}

void FFT::setWindow(FFT::IEWindowType type)
{
    std::vector<awe::Afloat> window = generate_window(type, mFrames);
    mWindow.swap(window);

    // Calculate window scaling
    for (ulong t = 0; t < mFrames; ++t)
        mWindowScale += mWindow[t];

    mWindowScale = (mWindowScale > 0.0f) ? 4.0f / (mWindowScale * mWindowScale) : 1.0f;
}


void FFT::calc_FFT(const awe::AfBuffer& buffer)
{
    /* Read input and apply window */
    for (ulong t = 0; t < mFrames; t++)
    {
        kIl[t] = buffer.at(t*2)   * mWindow[t];
        kIr[t] = buffer.at(t*2+1) * mWindow[t];
    }

    kiss_fftr( kCl , kIl , kOl );
    kiss_fftr( kCr , kIr , kOr );

    ulong const n = mFrames / 2; /* Look for: Nyquist Theorem */

    /* Get magnitude of FFT */
    for (ulong t = 0; t < n; ++t)
    {
        float rl, il, rr, ir;

        rl = kOl[t].r;
        il = kOl[t].i;

        rr = kOr[t].r;
        ir = kOr[t].i;

        mOutput[t*2  ] = rl*rl + il*il;
        mOutput[t*2+1] = rr*rr + ir*ir;
    }
}


void FFT::update(awe::AfBuffer const& buffer)
{
    std::lock_guard<std::mutex> lock(mMutex);

    calc_FFT(buffer);

    for(ulong i = 0; i < mBands; i++)
    {
        unsigned a = ceil (mBandX[i  ]);
        unsigned b = floor(mBandX[i+1]);
        unsigned c = a - 1;

        awe::Asfloatf n({ 0.0f, 0.0f });
        awe::Asfloatf o(mSpectrum.data() + i*2);

        /* Transform frequency axis from linear scale to logarithmic scale */

        // where x = 0 .. b
        if (b < a) { // log(x) / log(b) < 1.0;
            n[0] += mOutput.at(b*2  ) * (mBandX[i+1] - mBandX[i]),
            n[1] += mOutput.at(b*2+1) * (mBandX[i+1] - mBandX[i]);
        } else {     // log(x) / log(b) > 1.0;
            if (a > 0)
                n[0] += mOutput.at(c*2  ) * (a - mBandX[i]),
                n[1] += mOutput.at(c*2+1) * (a - mBandX[i]);

            for (; a < b; a++)
                n[0] += mOutput.at(a*2  ),
                n[1] += mOutput.at(a*2+1);

            if (b < mBands)
                n[0] += mOutput.at(b*2  ) * (mBandX[i+1] - b),
                n[1] += mOutput.at(b*2+1) * (mBandX[i+1] - b);
        }

        /* Apply magnitude scaling on spectra value. */
        switch (mScaleType)
        {
            case FFT::IEScaleType::DBFS:
                n( [this] (float &x) {
                        x = 10.0f * log10(x * mWindowScale);
                        x = (x == x) ? ( (x > -mRange) ? (x + mRange) : 0.0f ) : 0.0f;
                        } );
                break;

            case FFT::IEScaleType::CUBIC:
                n( [] (float &x) { x = powf(x, 1.0f/3.0f); });
                n *= mRange;
                break;

            case FFT::IEScaleType::LINEAR:
                n( [] (float &x) { x = sqrt(x); });
                n *= mRange * 2.0f;
                break;
        }

        // Apply peaking and decay on Old Peak
        o[0] -= mRange * mDecay,
        o[1] -= mRange * mDecay;

        // Old Peak value clipping
        o[0] = o[0] > 0.0f ? o[0] : 0.0f,
        o[1] = o[1] > 0.0f ? o[1] : 0.0f;

        // Use New Peak if it is louder
        n[0] = n[0] > o[0] ? n[0] : o[0],
        n[1] = n[1] > o[1] ? n[1] : o[1];

        mSpectrum[i*2  ] = n[0];
        mSpectrum[i*2+1] = n[1];
    }
}

void FFT::update_2(awe::AfBuffer const &buffer) {
    auto        pvec = mPrevious;
    auto const  nvec = buffer;

    uint s = buffer.size() / 2;

    std::copy(pvec.cbegin() + s, pvec.cend(), pvec.begin());
    std::copy(nvec.cbegin(), nvec.cbegin() + s, pvec.begin() + s);

    update(mPrevious);
    update(buffer);

    mPrevious = buffer;

    set_needs_render();
}

////    clan::View methods    ////////////////////////////////////
void FFT::render_content(clan::Canvas &canvas)
{
    std::lock_guard<std::mutex> lock(mMutex);

    float bandw = box_style.height() / static_cast<float>(mBands);
    float midpt = box_style.width () / 2.0f;

    // Draw metering lines
    canvas.draw_line(
        midpt, 0, midpt, box_style.height(),
        clan::Colorf(1.0f, 1.0f, 1.0f, 0.05f)
        );

    const uchar n = 4;
    const float l = awe::dBFS_limit / n;

    for (int i = 1; i <= 4; ++i)
    {
        float j = static_cast<float>(i) * l;
        canvas.draw_line(
            midpt - j, 0, midpt - j, box_style.height(),
            clan::Colorf(1.0f, 1.0f, 1.0f, 0.01f)
            );
        canvas.draw_line(
            midpt + j, 0, midpt + j, box_style.height(),
            clan::Colorf(1.0f, 1.0f, 1.0f, 0.01f)
            );
    }

    // Draw spectrum
    clan::ColorHSVf color(0.0f, 1.0f, 1.0f, 0.1f);
    float inc = 270.0f / mBands;

    for(ulong i = 0; i < mBands; ++i)
    {
        color.h += inc;

        rectf out {
            midpt - mSpectrum.at(i*2  ), bandw * (mBands-i  ),
            midpt + mSpectrum.at(i*2+1), bandw * (mBands-i+1)
        };

        canvas.fill_rect(out, color);

        const float vec = 3.0f * ( mSpectrum.at(i*2+1) - mSpectrum.at(i*2  ) );
        canvas.fill_triangle(
                point2f{ midpt + vec          , bandw * (mBands-i+2) },
                point2f{ midpt + mVector.at(i), bandw * (mBands-i+1) },
                point2f{ midpt + vec          , bandw * (mBands-i  ) },
                clan::Colorf { 1.0f, 1.0f, 1.0f, 0.2f }
                );

        mVector[i] = vec;
    }
}


}
