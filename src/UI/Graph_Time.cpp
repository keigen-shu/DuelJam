#include "Graph_Time.hpp"

namespace UI {

Graph_Time::Graph_Time(
		clan::Colorf const &plot_color,
		clan::Colorf const &curr_color,
		Graph::PlotType const &type
) : Graph(plot_color, curr_color, type),
	mLastStamp(Graph_Time::Clock::now())
{ }

void Graph_Time::set_time(ushort const &skip)
{
	auto   const t = Graph_Time::Clock::now();
	ushort const s = std::chrono::duration_cast<Graph_Time::TimeUnit>(t - mLastStamp).count() / skip;

	for(uchar i = skip; i > 0; i -= 1)
		set_next(s);

	mLastStamp = t;
}

}
