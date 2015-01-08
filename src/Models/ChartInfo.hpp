//  Models/ChartInfo.hpp :: Chart information model
//  Copyright 2011 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_CHART_INFO_H
#define MODEL_CHART_INFO_H

#include <string>

struct ChartInfo
{
	std::string     name;      //!< Name of this chart
	std::string     charter;   //!< Name of the person who made this chart
	unsigned int    level;     //!< Difficulty rating number
	unsigned int    cc_count;  //!< Number of control change events.
	unsigned int    ne_count;  //!< Number of note events.
	unsigned int    duration;  //!< Length of the chart in seconds.
	double          tempo;     //!< Starting tempo of this chart.
};

#endif
