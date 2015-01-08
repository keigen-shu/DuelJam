//  Filters/IIR.hpp :: Infinite Impulse Response Filter
//  Copyright (c) 2014 Keigen Shu

#ifndef AWE_FILTER_IIR_H
#define AWE_FILTER_IIR_H

#include "../Filter.hpp"
#include <array>

namespace awe {
namespace Filter {

/**
 * 2nd-order IIR filter.
 *
 * Filtering is performed using direct transpose canonical realization.
 *
 * Based on the book 'Digital Filter Design' by Zoran Milivojevic.
 *
 *      http://www.mikroe.com/products/view/268/digital-filter-design/
 */
namespace IIR
{
    using PartialCoeffs = std::array<double, 3>;    //<! Partial coefficient array
    using        Coeffs = std::array<double, 6>;    //<! IIR filter coefficient array

    using DelayLine = std::array<double, 2>;        //<! Channel delay line

    /** Constructs a Low Pass Filter.
     */
    Coeffs newLPF(const double rate, const double freq) noexcept;

    /** Constructs a High Pass Filter.
     */
    Coeffs newHPF(const double rate, const double freq) noexcept;

    void process_one
            ( PartialCoeffs const & b
            , PartialCoeffs const & a
            , DelayLine & z
            , double & x
            ) noexcept;

    template< const Achan Channels >
    struct IIR
    {
        PartialCoeffs                   mB, mA;
        std::array<DelayLine, Channels> mZ;

        IIR(Coeffs k) noexcept
            : mB ( { k[0], k[1], k[2] } )
            , mA ( { k[3], k[4], k[5] } )
        { reset(); }

        /// Resets the filter's processing state.
        inline void reset() noexcept
        {
            for(DelayLine& z : mZ)
                z.fill(0.);
        }

        inline void process(const Achan c, double &v) noexcept {
            process_one(mB, mA, mZ[c], v);
        }

        inline void process(AfBuffer& buffer) noexcept
        {
            assert(buffer.size() % Channels == 0);

            Afloat* x = buffer.data();
            for(AfBuffer::size_type i = 0; i < buffer.size(); i++)
            {
                double v = x[i];
                process_one(mB, mA, mZ[i % Channels], v);
                x[i] = static_cast< Afloat >(v);
            }
        }

    };

};

}
}

#endif
