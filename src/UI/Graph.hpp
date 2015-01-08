//  UI/Graph.hpp :: Value graphing object for logging purposes.
//  Copyright 2014 Chu Chin Kuan

#ifndef UI_GRAPH_H
#define UI_GRAPH_H

#include "../__zzCore.hpp"

namespace UI {

class Graph : public clan::View
{
public:
	/** Graph plotting style enumerator */
	enum PlotType : uchar {
		BAR     = 'B',
		LINE    = 'L',
		POINT   = 'P'
	};

	using PlotArray = std::array<ushort, 256>;

protected:
	clan::Colorf    mcPlot, mcThis;
	PlotType        mType;
	PlotArray       mArray;
	uchar           mNextIndex;

public:
	Graph(
			clan::Colorf const &plot_color = clan::Colorf { 1.0f, 1.0f, 1.0f, 0.6f },
			clan::Colorf const &curr_color = clan::Colorf { 1.0f, 0.0f, 0.0f, 0.8f },
			Graph::PlotType const &type    = Graph::PlotType::POINT
	     );

	void set_next(ushort const &value);

	inline void set_color(clan::Colorf const &plot, clan::Colorf const &curr) {
		mcPlot = plot, mcThis = curr;
	}

	inline void set_type(PlotType const &type) {
		mType = type;
	}

	void render_content(clan::Canvas &canvas) override;
};

}
#endif
