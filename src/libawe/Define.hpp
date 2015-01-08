//  Define.hpp :: Essential declarations and definitions
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_DEFINE
#define AWE_DEFINE

#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
# include <cstdint>

#include <queue>
#include <vector>

//! \brief The libawe namespace, where everything used by libawe resides in.
namespace awe
{

//!@name Standard audio data types
//!@{
using Aint   = int16_t; //!< 16-bit integer data type
using Afloat = float;   //!< Architecture-specific floating point data type

using Achan  = uint8_t; //!< Channel count data type.
//!@}

//!@name Standard audio data containers
//!@{
template< typename T >
using Abuffer       = std::vector<T>;
template< typename T >
using AFIFObuffer   = std::queue <T>;

using AiBuffer      = Abuffer<Aint  >;
using AfBuffer      = Abuffer<Afloat>;

using AiFIFOBuffer  = AFIFObuffer<Aint  >;
using AfFIFOBuffer  = AFIFObuffer<Afloat>;
//!@}

#define IO_BUFFER_SIZE  16384   //!< Default file IO buffer size

//!@name Standard data type converters
//!@{

//! @brief 16-bit bi-polar integer to normalized floating point value casting function.
//! @return Afloat equivalent of Aint
inline Afloat to_Afloat(Aint v)
{
    if (v == 0) {
        return 0;
    } else if (v < 0) {
        return float(v) / 32768.0f;
    } else {
        return float(v) / 32767.0f;
    }
}

//! @brief Normalized floating point to 16-bit bi-polar integer value casting function.
//! @return Aint equivalent of Afloat
inline Aint to_Aint(Afloat v)
{
    if (v != v) { // Abnormal floating point
        return 0;
    } else if (v < 0) {
        return std::max<>(static_cast<Aint>(v * 32768.0f), static_cast<Aint>(-32768));
    } else {
        return std::min<>(static_cast<Aint>(v * 32767.0f), static_cast<Aint>(32767));
    }
}
//!@}

//! Converts a normalized linear magnitude value (0.0f - 1.0f) to a full-scale decibel magnitude value
inline Afloat   to_dBFS(Afloat v)
{
    return 20.0f * log10(v);
}

//! Converts a full-scale decibel magnitude value to a normalized linear magnitude value (0.0f - 1.0f)
inline Afloat from_dBFS(Afloat v)
{
    return pow(10.0f, v / 20.0f);
}

//! Smallest unit of representation for a 16-bit integer on a normalized floating point.
static constexpr Afloat int16_normalized_epsilon = 1.0f / 65535.0f;

//! Full scale decibel limit (~-96 dB).
static Afloat dBFS_limit = to_dBFS(int16_normalized_epsilon);

//! General purpose audio rendering configuration structure passed to modules.
struct ArenderConfig {
    //! Render quality option
    enum class Quality : uint8_t
    {
        DEFAULT  = 0x0, //!< Default rendering configuration.
        FAST     = 0x1, //!< Fastest rendering option.
        MEDIUM   = 0x2, //!< Balanced rendering option.
        BEST     = 0x3, //!< Best render quality.
        MUTE     = 0xE, //!< Update renderer without doing anything to the target buffer.
        SKIP     = 0xF  //!< Return immediately without doing anything.
    };

    /** Target stream sampling rate. */
    unsigned long sampleRate;

    /** Number of frames to render onto target buffer. */
    unsigned long frameCount;

    /** Index on the target buffer to start writing from. */
    unsigned long frameOffset;

    /** Rendering quality. */
    Quality quality;

    /** Default constructor. */
    ArenderConfig(
        unsigned long sample_rate,
        unsigned long frame_count,
        unsigned long frame_offset = 0,
        Quality q = Quality::DEFAULT
    )   : sampleRate(sample_rate)
        , frameCount(frame_count)
        , frameOffset(frame_offset)
        , quality(q)
    { }

};

/** Interpolation function
 *
 * An interpolation filter is used to upsample an audio buffer.
 *
 * 4-point, 4th-order optimal 4x z-form interpolation function
 * by Olli Niemitalo, link: http://yehar.com/blog/?p=197
 */
template <
class T,
      class = typename std::enable_if< std::is_floating_point<T>::value >::type
      > // Define for floating point types only
T interpolate_4p4o_4x_zform(
    T const& x,
    T const& y0, // y[-1]
    T const& y1, // y[ 0] <- x is between
    T const& y2, // y[ 1] <- these values
    T const& y3  // y[ 2]
)
{
    T const z = x - 1.0 / 2.0;

    T const e1 = y2 + y1, o1 = y2 - y1;
    T const e2 = y3 + y0, o2 = y3 - y0;

    T const c0 = e1 *  0.46567255120778489 + e2 *  0.03432729708429672;
    T const c1 = o1 *  0.53743830753560162 + o2 *  0.15429462557307461;
    T const c2 = e1 * -0.25194210134021744 + e2 *  0.25194744935939062;
    T const c3 = o1 * -0.46896069955075126 + o2 *  0.15578800670302476;
    T const c4 = e1 *  0.00986988334359864 + e2 * -0.00989340017126506;

    return (((c4 * z + c3) * z + c2) * z + c1) * z + c0;
}


} // namespace awe

#endif // AWE_DEFINE
