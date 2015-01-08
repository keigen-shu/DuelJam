//  Models/Note.hpp :: Note object model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_NOTE_H
#define MODEL_NOTE_H

#include "ChronoTTime.hpp"
#include "NoteKey.hpp"

struct Note
{
	ENoteKey k; //!< Designated key.
	TTime    t;

	constexpr Note(const ENoteKey &key, const TTime &time)
		: k(key), t(time) { }

	constexpr Note(const Note &other)
		: k(other.k), t(other.t) { }
};

#endif
