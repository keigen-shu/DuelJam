//  Model/Sequence.cpp :: Music sequence model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#include <cassert>
#include "Sequence.hpp"
#include "NoteAlgorithm.hpp"

void populateIndices(Sequence &sequence)
{
	tick_count_t count = 0;
	for(Measure &m : sequence)
	{
		assert(m.tick_count == getTickCount(m.signature));
		m.tick_index = count;
		count += m.tick_count;
	}
}

Sequence::size_type getMeasureIndex(const Sequence &sequence, tick_count_t tick)
{
	Sequence::size_type index = 0;
	for(const Measure &m : sequence)
	{
		if (m.tick_index > tick)
			return index;
		else
			index += 1;
	}

	return 0;
}

Measure::CCs extractCCs(const Sequence &sequence)
{
	Measure::CCs e;
	for(const Measure &m : sequence)
		e.insert(e.cend(), m.mCCs.cbegin(), m.mCCs.cend());

	std::sort(e.begin(), e.end(), cmpEventCC_Greater);
	return e;
}

Measure::NEs extractNEs(Sequence &sequence)
{
	Measure::NEs e;
	for(Measure &m : sequence) {
		Measure::NEs me = m.getNoteEvents();
		e.insert(e.cend(), me.cbegin(), me.cend());
	}

	return e;
}

std::map<ENoteKey, Measure::NEs> splitByKey(Measure::NEs &sequence)
{
	std::map<ENoteKey, Measure::NEs> e;

	for(EventNote* &n : sequence)
	{
		e[n->k].push_back(n);
	}

	return e;
}

long int getTDistance(const Sequence &sequence, TTime a, TTime b)
{
	// Swap places if b comes earlier than a.
	if (a >  b) return -(getTDistance(sequence, b, a));
	else if (a == b) return 0;

	tick_delta_t r = 0;

	// Count from starting point to the end
	for(unsigned int i = a.m; i <= b.m; i++)
	{
		const Measure &m = sequence.at(i);

		if (a.m == b.m) {
			r += (b.b * m.getB() + b.t) - (a.b * m.getB() + a.t);
		} else if (i == a.m) {
			r += (m.getA() -(a.b+1)) * m.getB();
			r +=  m.getB() - a.t;
		} else if (i == b.m) {
			r += (b.b* m.getB());
			r +=  b.t;
		} else {
			r += m.tick_count;
		}
	}

	return r;
}

