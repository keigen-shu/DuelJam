//  Model/Measure.hpp :: Music segment model
//  Copyright 2011 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_MEASURE_H
#define MODEL_MEASURE_H

#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>
#include "EventCC.hpp"
#include "EventNoteInstance.hpp"

struct Measure
{
	typedef std::vector<EventCC>            CCs; //!< CC event container typedef
	typedef std::vector<EventNote*>         NEs; //!< Note event container typedef
	typedef std::vector<EventNoteSingle>    NSs;
	typedef std::vector<EventNoteLong>      NLs;

	TSignature      signature;

	/*! Pre-calculated length (in number of ticks) of this measure. This value
	 *  has to be manually updated when `signature` is modified.
	 */
	tick_count_t    tick_count;

	/*! Pre-calculated starting tick-position of this measure inside a music
	 *  `Sequence`. This attribute is used solely for the purpose of keeping
	 *  TEMPORARILY the tick position of this measure inside the sequence.
	 */
	tick_count_t    tick_index;

	CCs mCCs;
	NSs mNSs;
	NLs mNLs;

	Measure(TSignature s)
		: signature(s), tick_count(getTickCount(s)) { }

	Measure(double z)
		: Measure(getTimeSignature(z)) { }

	inline beat_count_t getA() const { return signature.first; }
	inline tick_count_t getB() const { return signature.second; }

	void setSignature(const TSignature &new_value);

	template< class... Args >
		void addNoteSingle(Args... args) { mNSs.emplace_back(std::forward<Args>(args)...); }

	template< class... Args >
		void addNoteLong  (Args... args) { mNLs.emplace_back(std::forward<Args>(args)...); }

	void sort_elements();

	NEs getNoteEvents();
};

#endif
