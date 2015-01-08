//  Models/Chart.hpp :: Music chart model
//  Copyright 2011 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_CHART_H
#define MODEL_CHART_H

#include <ClanLib/display.h>
#include "../__zzCore.hpp"
#include "../AudioManager.hpp"
#include "ChartInfo.hpp"
#include "Sequence.hpp"

class Chart
{
protected:
	ChartInfo mInfo;      //!< Information about this chart.

	clan::PixelBuffer           mCover;     //!< Cover art pixel buffer.
	std::shared_ptr<Sequence>   mSequence;  //!< Event sequence container.
	std::shared_ptr<SampleMap>  mSampleMap; //!< ID to Sample mapping container.

public:
	Chart()
		: mInfo     ()
		, mCover    ()
		, mSequence (std::make_shared<Sequence>())
		, mSampleMap(std::make_shared<SampleMap>())
	{ }
	virtual ~Chart() { }

	virtual void load_cover_art() = 0;
	virtual void load_chart    () = 0;
	virtual void load_samples  () = 0;

	void load_all     ();
	inline void sort_sequence() { for(Measure & measure : *mSequence) { measure.sort_elements(); } }

	inline ChartInfo const & cgetInfo() const { return mInfo; }
	inline ChartInfo       &  getInfo()       { return mInfo; }

	inline clan::PixelBuffer    getCoverArt () { return mCover; }
	inline std::shared_ptr<Sequence > const & cgetSequence () const { return mSequence; }
	inline std::shared_ptr<Sequence >       &  getSequence ()       { return mSequence; }
	inline std::shared_ptr<SampleMap> const & cgetSampleMap() const { return mSampleMap; }
	inline std::shared_ptr<SampleMap>       &  getSampleMap()       { return mSampleMap; }
};

#endif
