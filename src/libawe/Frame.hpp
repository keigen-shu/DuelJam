//  Frame.hpp :: Audio frame template
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_FRAME_H
#define AWE_FRAME_H

#include "Define.hpp"

#include <algorithm>
#include <array>

namespace awe
{

/*! Structure holding a single audio frame.
 *
 *  A frame is a snapshot of all sound samples on all channels at a specific
 *  time point. This structure is designed as an expansion of the STL array to
 *  support array/frame-wide algorithms and arithmetic.
 *
 *  @tparam T type of data that this structure will hold.
 *  @tparam Channels number of channels of data within this single frame.
 */
template< typename T, Achan Channels >
struct Aframe {
    using container_type = std::array< T, Channels >;
    using value_type     = typename container_type::value_type;

    container_type data;

    Aframe() : data() { }
    Aframe(const T* _data, Achan _size = Channels) : data() {
        std::copy(_data, _data + std::min(_size, Channels), data.begin());
    }

    Aframe(const container_type&  _data) : data(_data) {}
    Aframe(const container_type && _data) : data(_data) {}

    T& operator[](const Achan& pos) {
        return data[pos];
    }

    static Aframe from_buffer(Abuffer<T> const& buffer, size_t frame) {
        assert(buffer.size() / Channels > frame);
        return Aframe(buffer.data() + (frame * Channels), Channels);
    }

    void to_buffer(Abuffer<T>& buffer, size_t frame) const {
        assert(buffer.size() / Channels > frame);
        T* buff = buffer.data() + (frame * Channels);
        for (Achan c = 0; c < Channels; c += 1) {
            buff[c] = data[c];
        }
    }

    /* ARITHMETIC */
    void operator+= (const T& v) {
        for (T & u : data) {
            u += v;
        }
    }
    void operator-= (const T& v) {
        for (T & u : data) {
            u -= v;
        }
    }
    void operator*= (const T& v) {
        for (T & u : data) {
            u *= v;
        }
    }
    void operator/= (const T& v) {
        for (T & u : data) {
            u /= v;
        }
    }

    template< Achan channels >
    Aframe operator+ (Aframe<T, channels> v) const {
        Aframe f;
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            f.data[c] = data[c] + v[c];
        }
        return f;
    }
    template< Achan channels >
    Aframe operator- (Aframe<T, channels> v) const {
        Aframe f;
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            f.data[c] = data[c] - v[c];
        }
        return f;
    }
    template< Achan channels >
    Aframe operator* (Aframe<T, channels> v) const {
        Aframe f;
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            f.data[c] = data[c] * v[c];
        }
        return f;
    }
    template< Achan channels >
    Aframe operator/ (Aframe<T, channels> v) const {
        Aframe f;
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            f.data[c] = data[c] / v[c];
        }
        return f;
    }

    template< Achan channels >
    void operator+= (Aframe<T, channels> v) {
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            data[c] += v[c];
        }
    }

    template< Achan channels >
    void operator-= (Aframe<T, channels> v) {
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            data[c] -= v[c];
        }
    }

    template< Achan channels >
    void operator*= (Aframe<T, channels> v) {
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            data[c] *= v[c];
        }
    }

    template< Achan channels >
    void operator/= (Aframe<T, channels> v) {
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            data[c] /= v[c];
        }
    }


    template< Achan channels >
    void operator+= (std::array<T, channels> v) {
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            data[c] += v[c];
        }
    }

    template< Achan channels >
    void operator-= (std::array<T, channels> v) {
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            data[c] -= v[c];
        }
    }

    template< Achan channels >
    void operator*= (std::array<T, channels> v) {
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            data[c] *= v[c];
        }
    }

    template< Achan channels >
    void operator/= (std::array<T, channels> v) {
        for (Achan c = 0; c < std::min<>(Channels, channels); ++c) {
            data[c] /= v[c];
        }
    }

    /* ALGORITHMS */
    template <typename Function>
    void operator()(Function F) {
        for (T & u : data) {
            F(u);
        }
    }

    void abs() {
        for (T & u : data) {
            u = std::abs(u);
        }
    }

#ifdef MSC_VER
    const T& max() const {
        return std::max<>(data.cbegin(), data.cend());
    }

    const T& min() const {
        return std::min<>(data.cbegin(), data.cend());
    }
#endif

    T absmax() const {
        auto v = std::minmax_element(data.cbegin(), data.cend());
        return std::max<>(std::fabs(*v.first), std::fabs(*v.second));
    }

    T absmin() const {
        auto v = std::minmax_element(data.cbegin(), data.cend());
        return std::min<>(std::fabs(*v.first), std::fabs(*v.second));
    }
};

//!@name Standard libawe frame types
//!@{
typedef Aframe<Aint  , 2> Asintf;                           //!< a stereo Aint frame
typedef Aframe<Afloat, 2> Asfloatf;                         //!< a stereo Afloat frame
//!@}

//!@name Standard data type converters
//!@{
template< Achan Channels >
Aframe< Afloat, Channels > to_Afloatf(Aframe< Aint, Channels > i)
{
    Aframe< Afloat, Channels > f;
    for (Achan c = 0; c < Channels; c += 1) {
        f[c] = to_Afloat(i[c]);
    }
    return i;
}

template< Achan Channels >
Aframe< Aint, Channels > to_Aintf(Aframe< Afloat, Channels > f)
{
    Aframe< Aint, Channels > i;
    for (Achan c = 0; c < Channels; c += 1) {
        i[c] = to_Aint(f[c]);
    }
    return i;
}

template< Achan SChannels, Achan DChannels >
Aframe< Afloat, DChannels > to_Afloatf(Aframe< Aint, SChannels > i)
{
    Aframe< Afloat, DChannels > f;
    Achan C = std::min<>(SChannels, DChannels);
    for (Achan c = 0; c < C; c += 1) {
        f[c] = to_Afloat(i[c]);
    }
    return i;
}

template< Achan SChannels, Achan DChannels >
Aframe< Aint, DChannels > to_Aintf(Aframe< Afloat, SChannels > f)
{
    Aframe< Aint, DChannels > i;
    Achan C = std::min<>(SChannels, DChannels);
    for (Achan c = 0; c < C; c += 1) {
        i[c] = to_Aint(f[c]);
    }
    return i;
}
//!@}
//!@}

}

#endif