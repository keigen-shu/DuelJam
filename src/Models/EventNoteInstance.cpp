//  Models/EventNoteInstance.cpp :: Instantiated note event model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#include <ClanLib/display.h>
#include "EventNoteInstance.hpp"
#include "Tracker.hpp"


EventNoteSingle::EventNoteSingle(const EventNote &note, const NoteAudio &audio)
	: EventNote(note)
	, mAudio(audio)
{ }

EventNoteSingle::EventNoteSingle(const ENoteKey &key, const TTime &time, const NoteAudio &audio)
	: EventNote(key, time)
	, mAudio(audio)
{ }

void EventNoteSingle::init(const Tracker &tracker)
{
	mScore = JudgeScore{};
	mTick = getTDistance(tracker.cgetSequence(), TTime{}, t);
}

void EventNoteSingle::render(const Tracker &tracker, clan::Canvas &canvas) const
{

}

void EventNoteSingle::update(const Judge &judge, const tick_count_t &current_tick, const InputKeyStatus &stat, NoteAudio& audio)
{
	JudgeScore score = judge.judge(this->getTick() - current_tick);
	switch(stat)
	{
	case InputKeyStatus::AUTO:
		audio = getAudio();
		// am->play(mSampleID, ENKey_toInteger(getKey()), mVol, mPan);
		mScore = JudgeScore{ EJudgeRank::AUTO, 0, score.delta };
		mDead = true;
		return;
	case InputKeyStatus::ON:
		audio = getAudio();
		// am->play(mSampleID, ENKey_isPlayer1(getKey()) ? 1 : 2, mVol, mPan);
		if (score.rank == EJudgeRank::NONE)
		{
			return;
		} else {
			mScore = score;
			mDead = true;
			return;
		}
	case InputKeyStatus::OFF:
	case InputKeyStatus::LOCKED:
	default:
		if (score.rank == EJudgeRank::MISS) // Too late to hit.
		{
			mScore = score;
			mDead = true;
			return;
		} else {
			return;
		}
	}
}

EventNoteLong::EventNoteLong(const EventNoteSingle &b, const EventNoteSingle &e)
	: EventNote(b)
	, mBaudio(b.getAudio()) , mEaudio(e.getAudio())
	, mBscore(b.getScore()) , mEscore(e.getScore())
	, mBtick (b.getTick())  , mEtick (e.getTick())
{ }

void EventNoteLong::init(const Tracker &tracker)
{
	mBscore = JudgeScore{};
	mEscore = JudgeScore{};

	mBtick  = getTDistance(tracker.cgetSequence(), TTime{}, mBtick);
	mEtick  = getTDistance(tracker.cgetSequence(), TTime{}, mEtick);

	mScore  = JudgeScore{};
	mTick   = mBtick;
}

void EventNoteLong::render(const Tracker &tracker, clan::Canvas &canvas) const
{

}

void EventNoteLong::update(const Judge &judge, const tick_count_t &current_tick, const InputKeyStatus &stat, NoteAudio& audio)
{
	JudgeScore b_temp = JudgeScore();
	JudgeScore e_temp = JudgeScore();

	b_temp = judge.judge(mBtick - current_tick);
	e_temp = judge.judge(mEtick - current_tick);


	// Remove from key-lock context if score is already set.
	// But stay alive if not past deletion point.
	if (mScore.rank != EJudgeRank::NONE) {
		mDead = (e_temp.rank == EJudgeRank::MISS) ? true : mDead;
		return;
	}

	//// WAIT -> Starting point not hit yet. Respond to key status.
	//// LIVE -> Starting point hit, but end point hasn't. Respond to key status.
	//// DONE -> Note::score is set, but not dead as we still need to render graphics.
	//// DEAD -> Note::dead  is set.
	//// TODO Make this prettier and less redundant.
	switch(stat)
	{
	case InputKeyStatus::AUTO: // [DONE] Autoplay note.
		if (mBscore.rank == EJudgeRank::NONE) {
			// am->play(mBSID, ENKey_toInteger(getKey()), mVol, mPan);
			audio = getAudio();
			mBscore = JudgeScore( EJudgeRank::AUTO, 0, b_temp.delta );
		}

		if (mEscore.rank == EJudgeRank::NONE) {
			mEscore = JudgeScore( EJudgeRank::AUTO, 0, e_temp.delta );
		}

		if (e_temp.delta <= 0) {
			mScore  = JudgeScore( EJudgeRank::AUTO, 0, 0 );
			mDead = true;
		}

		return;

	case InputKeyStatus::LOCKED: // Holding Key
		if (mEscore.rank == EJudgeRank::NONE) {         // Unscored end
			if (mBscore.rank != EJudgeRank::NONE
					&&  mBscore.rank != EJudgeRank::MISS
					&&  mBscore.rank != EJudgeRank::AUTO) {     // Scored starting point
				assert(mScore.rank == EJudgeRank::NONE && "Note logic leak.");
				if (e_temp.rank == EJudgeRank::MISS) {          // [DONE] Too late to release
					mEscore = e_temp;
					calc_score();
					mDead = true;
					return;
				} else {                            // [LIVE] Still waiting for end point
					return;
				}
			} else if (mBscore.rank == EJudgeRank::MISS
					|| mBscore.rank == EJudgeRank::AUTO) {      // Missed starting point
				mEscore = mBscore;                  // This should not be needed, but someone kept forgetting to set mEscore somewhere.
				calc_score();
				return;
			} else {                                        // Unscored starting point
				if (b_temp.rank == EJudgeRank::MISS) {  // [DONE] Missed starting point.
					mBscore = b_temp;
					mEscore = b_temp;
					calc_score();
					return;
				} else {                            // [WAIT] Still have the time to respond.
					return;
				}
			}
			// Key lock belongs to another note... Same effect as being OFF.
		} else { return; }                                  // [DONE] Scored end
		break;

	case InputKeyStatus::OFF : // Have not hit anything OR released key.
		if (mBscore.rank == EJudgeRank::NONE) {     // Unscored starting point; not active yet.
			if (b_temp.rank == EJudgeRank::MISS) {  // [DONE] Missed starting point.
				mBscore = b_temp;
				mEscore = b_temp;
				calc_score();
				return;
			} else {                            // [WAIT] Still have the time to respond.
				return;
			}
		} else if (mBscore.rank == EJudgeRank::MISS
				|| mBscore.rank == EJudgeRank::AUTO) {      // Starting point was scored MISS or AUTO
			assert(mBscore.rank == mEscore.rank);       // Starting point and ending points must be equal.
			if (e_temp.rank == EJudgeRank::MISS) {  // [DEAD] Past target time
				mDead = true;
				return;
			} else {                            // [DONE] Not past target time
				return;
			}
		} else {                                // Starting point was scored.
			if (mEscore.rank == EJudgeRank::NONE) {         // Ending point hasn't, so the player was holding this.
				if (e_temp.rank == EJudgeRank::NONE) {              // [DONE] Release too early
					mEscore = JudgeScore( EJudgeRank::MISS, 0, e_temp.delta );
					calc_score();
					return;
				} else {                                        // [DEAD] Release at the right time
					mEscore = e_temp;
					calc_score();
					return;
				}
			} else {                                    // Ending point was scored.
				if (e_temp.rank == EJudgeRank::MISS) {              // [DEAD] Past target time
					mDead = true;
					return;
				} else {                                        // [DONE] Not past target time
					return;
				}
			}
		}

	case InputKeyStatus::ON: // Just hit the key or rehit after miss.
		if (mBscore.rank == EJudgeRank::NONE) {             // Starting point was not hit
			//am->play(mBSID, ENKey_isPlayer1(getKey()) ? 1 : 2, mVol, mPan);  // Play sound.
			audio = getAudio();
			if (b_temp.rank == EJudgeRank::NONE) {                  // [WAIT] Hit too early
				return;
			} else if (b_temp.rank == EJudgeRank::MISS
					|| b_temp.rank == EJudgeRank::AUTO) {           // [DONE] Rare case of MISS right when the key is hit.
				mBscore = b_temp;
				mEscore = b_temp;
				calc_score();
				return;
			} else {                                            // [LIVE] Staring point scores!
				mBscore = b_temp;
				return;                                     // DO NOT CLEAR FROM ACTIVE QUEUE
			}
		} else if (mEscore.rank == EJudgeRank::NONE) {      // Starting point was hit, ending point hasn't
			if (e_temp.rank == EJudgeRank::MISS) {                  // [DEAD] Past target time
				mEscore = e_temp;
				calc_score();
				mDead = true;
				return;
			} else {                                            // [DONE] Not past target time
				// am->play(mBSID, ENKey_isPlayer1(getKey()) ? 1 : 2, mVol, mPan);             // Play sound.
				audio = getAudio();
				return;
			}
		} else {
			return;
		}

	default:
		return;
	}
}

void EventNoteLong::calc_score()
{
    mScore.rank  = mEscore.rank;
    mScore.score = mEscore.score + mBscore.score;
    mScore.delta = mEscore.delta + mBscore.delta;
}
