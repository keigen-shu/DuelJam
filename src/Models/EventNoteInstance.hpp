//  Models/EventNoteInstance.hpp :: Instantiated note event model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_EVENT_NOTE_INSTANCE_H
#define MODEL_EVENT_NOTE_INSTANCE_H

#include "EventNote.hpp"

class EventNoteSingle : public EventNote
{
private:
	NoteAudio   mAudio;

public:
	EventNoteSingle(const EventNote &note, const NoteAudio &audio);
	EventNoteSingle(const ENoteKey &key, const TTime &time, const NoteAudio &audio);
	virtual ~EventNoteSingle() { }

	const NoteAudio& getAudio() const { return mAudio; }

	virtual void init  (const Tracker&) override;
	virtual void render(const Tracker&, clan::Canvas&) const override;
	virtual void update(const Judge&, const tick_count_t&, const InputKeyStatus&, NoteAudio&) override;
};

class EventNoteLong : public EventNote
{
public:
	enum class Edge : unsigned char {
		BEGIN = 'B',
		END   = 'E',
		WHOLE = 'W',
	};

private:    ////    ATTRIBUTES
	NoteAudio   mBaudio, mEaudio;
	JudgeScore  mBscore, mEscore;
	long        mBtick , mEtick;

public:
	EventNoteLong(const EventNoteSingle& a, const EventNoteSingle& b);
	virtual ~EventNoteLong() { }

	const NoteAudio& getAudio() const { return mEscore.rank == EJudgeRank::NONE ? mBaudio : mEaudio; }

	virtual void init  (const Tracker&) override;
	virtual void render(const Tracker&, clan::Canvas&) const override;
	virtual void update(const Judge&, const tick_count_t&, const InputKeyStatus&, NoteAudio&) override;

	void calc_score();
};

#endif
