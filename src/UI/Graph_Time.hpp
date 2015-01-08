//  UI/Graph_Time.hpp :: Duration graphing object
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef UI_GRAPH_TIME_H
#define UI_GRAPH_TIME_H

#include <chrono>
#include "Graph.hpp"

namespace UI {

/** Time-based graphing object.
 * Records and graphs the time taken to perform a task.
 */
class Graph_Time : public Graph
{
protected:
	using Clock         = std::chrono::steady_clock;
	using TimeStamp     = std::chrono::time_point<Clock>;
	using TimeUnit      = std::chrono::milliseconds;

	TimeStamp   mLastStamp;

public:
	Graph_Time(
			clan::Colorf const &plot_color = clan::Colorf { 1.0f, 1.0f, 1.0f, 0.6f },
			clan::Colorf const &curr_color = clan::Colorf { 1.0f, 0.0f, 0.0f, 0.8f },
			Graph::PlotType const &type    = Graph::PlotType::POINT
		);
	void set_time(ushort const &skip = 1);
};

}
#endif

