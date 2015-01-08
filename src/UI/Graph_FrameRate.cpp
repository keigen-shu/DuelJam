#include "Graph_FrameRate.hpp"

namespace UI {

Graph_FrameRate::Graph_FrameRate() : Graph_Time()
{
}

void Graph_FrameRate::render_content(clan::Canvas& canvas)
{
	set_time(1);
	Graph_Time::render_content(canvas);
	set_needs_render();
}

}
