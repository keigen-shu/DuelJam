//  Filters/Rack.hpp :: Mixer filter rack
//  Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_FILTER_RACK_H
#define AWE_FILTER_RACK_H

#include <cassert>
#include <cstdint>
#include "../Filter.hpp"

namespace awe {
namespace Filter {

template< Achan Channels >
class Rack : public Afilter< Channels >
{
public:
    using filter_type = Afilter< Channels >;

private:
    std::vector< filter_type* > filters;

public:
    Rack() {}

    inline void reset_state() override {
        for(filter_type* filter : filters)
            filter->reset_state();
    }

    inline void filter_buffer(AfBuffer &buffer) override {
        for(filter_type* filter : filters)
            filter->filter_buffer(buffer);
    }

    inline void attach_filter(filter_type* filter) { filters.push_back(filter); }
    inline void detach_filter(size_t       filter) {
        auto it = filters.begin();
        while (it != filters.end() || --filter > 0)
            it++;
        filters.erase(it);
    }

    inline filter_type       *  getFilter(size_t filter)       { return filters[filter]; }
    inline filter_type const * cgetFilter(size_t filter) const { return filters[filter]; }
};

}
}
#endif
