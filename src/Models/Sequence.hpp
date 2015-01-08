//  Model/Sequence.hpp :: Music sequence model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_SEQUENCE_H
#define MODEL_SEQUENCE_H

#include <map>
#include "Measure.hpp"

typedef std::vector<Measure> Sequence;

//! Populates the `tick_index` of each measure in the `Sequence`.
void populateIndices(Sequence &);

//! Finds the measure from an absolute tick index in the `Sequence`.
//! Requires that the sequence has tick indices populated.
Sequence::size_type getMeasureIndex(const Sequence&, tick_count_t);

Measure::CCs extractCCs(const Sequence&);
Measure::NEs extractNEs(Sequence&);

//! Obtains the tick-distance between two points of time in a sequence.
long int getTDistance(const Sequence &, TTime, TTime);

//! Obtains the tick-distance between two points of time in a sequence in
//! milliseconds.
double   getRDistance(const Sequence &, TTime, TTime);

#endif
