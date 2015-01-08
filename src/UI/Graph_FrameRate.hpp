//  UI/Graph_Time.hpp :: Frame rate grapher
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef UI_GRAPH_FRAMERATE_H
#define UI_GRAPH_FRAMERATE_H

#include "Graph_Time.hpp"

namespace UI {

/**
 * Frame rate graphing object.
 * Records the time taken to render a frame.
 */
class Graph_FrameRate : public Graph_Time
{
public:
	Graph_FrameRate();

	void render_content(clan::Canvas &canvas) override;
};

}
#endif
