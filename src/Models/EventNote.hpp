//  Models/EventNote.hpp :: Note event model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_EVENT_NOTE_H
#define MODEL_EVENT_NOTE_H

#include "InputKeyStatus.hpp"
#include "Judge.hpp"
#include "Note.hpp"
#include "NoteAudio.h"

namespace clan { class Canvas; }
class Tracker;

//! Class for notes with play state.
class EventNote : public Note
{
protected:    ////    STATE ATTRIBUTES
	bool        mDead;  //!< Is this note still active or not?
	JudgeScore  mScore; //!< Note scoring.

protected:    ////    CACHE ATTRIBUTES
	long        mTick;  //!< Tick position after chart initialization

public:
	EventNote(const ENoteKey &key, const TTime &time) noexcept
		: Note  (key, time)
		, mDead (false)
		, mScore()
		, mTick (-1)
	{ }

	EventNote(const EventNote &other) noexcept
		: Note  (other.k, other.t)
		, mDead (other.mDead)
		, mScore(other.mScore)
		, mTick (other.mTick)
	{ }

	virtual ~EventNote() { }

	inline const long         & getTick () const { return mTick; }
	inline const bool         & isDead  () const { return mDead; }
	inline const JudgeScore   & getScore() const { return mScore; }
	inline       bool           isScored() const { return mScore.rank != EJudgeRank::NONE; }

	// Initialize note tick position based on sequence (through tracker).
	virtual void init  (const Tracker &tracker) = 0;

	// TODO Move this to UI::Tracker::render<NoteType>;
	virtual void render(const Tracker &tracker, clan::Canvas &canvas) const = 0;
	virtual void update(const Judge &judge, const tick_count_t &, const InputKeyStatus &stat, NoteAudio &note) = 0;
};

#endif
