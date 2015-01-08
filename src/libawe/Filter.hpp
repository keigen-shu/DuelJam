//  Filter.hpp :: Filter class
//  Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_FILTER_H
#define AWE_FILTER_H

#include "Define.hpp"

namespace awe {

/* Define namespace for filters */
namespace Filter {}

/*! Audio filter interface.
 *  This class serves as an interface to all audio filtering objects.
 */
template< Achan Channels >
class Afilter
{
public:
    static constexpr Achan _channels = Channels;

    //! Virtual destructor
    virtual ~Afilter() { }

    //! Resets the filter to its initial state.
    virtual void reset_state() = 0;

    /*! Filters input as stereo-channeled Afloat sample buffer.
     *  @param[in,out] buffer buffer to filter through
     */
    virtual void filter_buffer(AfBuffer &buffer) = 0;
};

using AscFilter = Afilter<2>;

}
#endif
