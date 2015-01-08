//  Model/Tracker.cpp :: Music tracking model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#include "Tracker.hpp"
#include "Chart.hpp"
#include "EventNoteInstance.hpp"
#include "NoteKey.hpp"
#include "NoteAlgorithm.hpp"

Tracker::Tracker
	( ChartPtr      chart
	, Judge const & judge
	, KeyBindings   key_bindings
	, ClockPtr      ref_clock
	)
	: mJudge        (judge)
	, mRankScores   ()
	, mCombo(0), mMaxCombo(0)
	, mChart(chart)
	, mCCs  (extractCCs(getSequence()))
	, mNEs  (extractNEs(getSequence()))
	, mNextCC           (mCCs.begin())
	, mChannels         ()
	, mClock(ref_clock ? ref_clock : std::make_shared<TClock>(mChart->cgetInfo().tempo))
	, mTime (mClock->getTTime())
	, mCurrentTick      (0)
	, mChartEnded       (false)
	, mMeasureIterStart (0)
	, mAutoplay         (false)
	, mSpeedX           (1.0f)
{
	mJudge.calculateTiming(mClock->getTempo_mspt());

	// Initialize chart events.
	for(EventNote* note : mNEs)
		note->init(*this);

	for(EventCC  & cc : mCCs)
		cc.c = -cc.c;

	// Initialize measures.
	populateIndices(getSequence());

	// Initialize Channels
	if (key_bindings.empty())
	{
		mAutoplay = true;
	}
	else
	{
		for(Tracker::KeyBinding const &binding : key_bindings)
		{
			// Prevent many-key-to-one-channel and one-key-to-many-channel bindings.
			if (std::any_of(
						mChannels.cbegin(), mChannels.cend(),
						[&](const Channel &channel) -> bool {
						return (channel.key      == binding.first )
						|| (channel.key_code == binding.second);
						})
			   ) {
				fprintf(stderr, "Tracker [warn] Bad key-to-channel binding detected.\n");
				continue;
			}

			mChannels.push_back( Channel {
					.key        = binding.first,
					.key_code   = binding.second,
					.key_status = InputKeyStatus::OFF,
					.notes      = {}
					} );
		}
	}

	auto NEMap = splitNotes_byKey(mNEs);
	for(auto &node : NEMap)
	{
		auto it = std::find_if(
				mChannels.begin(), mChannels.end(),
				[&](const Tracker::Channel &ch) -> bool {
				return ch.key == node.first;
				});
		if (it == mChannels.end())
		{
			mChannels.push_back(Channel {
					.key        = node.first,
					.key_code   = clan::Key::none,
					.key_status = InputKeyStatus::OFF,
					.notes      = node.second
					});
		} else {
			it->notes = node.second;
		}
	}

	for (auto &channel : mChannels)
	{
		channel.next_note = channel.notes.begin();
	}
}

Tracker::ClockPtr const & Tracker:: getClock   () const { return mClock; }
Sequence          const & Tracker::cgetSequence() const { return *mChart->cgetSequence(); }
Sequence                & Tracker:: getSequence()       { return *mChart-> getSequence(); }
Tracker::NAs            & Tracker:: getNAs     ()       { return mNAs; }

void Tracker::update()
{
	fprintf(stderr, "Tracker::update() [%u:%u:%u].\n", mTime.m, mTime.b, mTime.t);

	mClock->update();

	if (mTime.m >= cgetSequence().size())
		mChartEnded = true;

	if (mChartEnded == false)
	{
		mCurrentTick = getTDistance(cgetSequence(), TTime(), mTime);
		mClock->setTCSig(cgetSequence().at(mTime.m).signature);
		updateCCs();
		updateNEs();
	}

}

void Tracker::updateCCs()
{
	if (mNextCC == mCCs.end()) return;

	if (mNextCC->t == this->mTime) {
		switch(mNextCC->c)
		{
			case EControl::CLOCK_TEMPO:
				fprintf(stderr, "CC CLOCK_TEMPO.\n");
				mClock->setTempo(mNextCC->v.asFloat);
				mJudge.calculateTiming(mClock->getTempo_mspt());
				break;
			case EControl::CLOCK_STOP_T:
				fprintf(stderr, "CC CLOCK_STOP_T.\n");
				mClock->setTStop(mNextCC->v.asInteger);
				break;
			default:
				if (isOff(mNextCC->c))
					fprintf(stderr, "[warn] CC not handled: Unknown / unimplemented control type '%u'.\n", mNextCC->c);
				else
					fprintf(stderr, "[warn] CC not handled: Control set as dead '-%u/%u'.\n",- mNextCC->c, mNextCC->c);
				break;
		}

		mNextCC->c = +mNextCC->c;
		mNextCC++;
	}

	if (mNextCC == mCCs.end()) return;

	if (mClock->cgetITime() > mNextCC->t ||
			mClock->cgetITime() < this->mTime
	   ) {
		mClock->setITime(mNextCC->t);
	} else if (mNextCC->t < mTime) {
		fprintf(stderr, "[warn] CC not handled on time.\n");
	}
}

void Tracker::updateNEs()
{
	for(Channel &channel : mChannels)
	{
		// Skip if no more notes.
		if (channel.next_note == channel.notes.end())
			continue;

		EventNote* note = *channel.next_note;
		if (// It's in autoplay channel or background channel.
				// or if AutoPlay is turned on
				ENoteKey_isAutoPlay(channel.key) || mAutoplay ||
				channel.key_code == clan::Key::none
				// ENoteKey_isAutoPlay(note->k) || mAutoplay
		   ) {
			if (note->t <= mTime)
			{
				NoteAudio aNote = makeEmpty();
				note->update(mJudge, mCurrentTick, InputKeyStatus::AUTO, aNote);
				if (isEmpty(aNote) == false)
					mNAs.push_back(aNote);

				// TODO Show note hit effect

				// Move to next note.
				channel.next_note++;
			} else {
				// Not time yet.
			}
		}
	}
}
