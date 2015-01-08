//  Chart_O2Jam.hpp :: O2Jam Chart object
//  Copyright 2011 - 2013 Keigen Shu

#ifndef CHART_O2JAM_H
#define CHART_O2JAM_H

#include "Models/Chart.hpp"
#include "Chart_O2Jam.hh"

struct Music;

namespace O2Jam {

class O2JamChart : public Chart
{
protected:
    std::string ojn_path;
    std::string ojm_path;

    OJN_Header  ojn_header;
    uint8_t     chart_index;

public:
    O2JamChart(const std::string &path, const OJN_Header &header, uint8_t index);

    virtual void load_cover_art () override;
    virtual void load_chart     () override;
    virtual void load_samples   () override;
};

Music* openOJN(const std::string &path);

}

#endif
