//  Model/Tracker.hpp :: Music tracking model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_TRACKER_H
#define MODEL_TRACKER_H

#include <memory>
#include <vector>
#include "../__zzCore.hpp"
#include "../AudioManager.hpp"
#include "ChronoTClock.hpp"
#include "Judge.hpp"
#include "Sequence.hpp"

class Chart;

class Tracker
{
public:
	using CCs = Measure::CCs;
	using NEs = Measure::NEs;

	using NAs = NoteAudioList;

	using InputKeyCode  = clan::Key;
	using RankScoreMap  = std::map<EJudgeRank, unsigned int>;

	struct Channel
	{
		ENoteKey        key;        //!< Key binding.
		InputKeyCode    key_code;   //!< Key code.
		InputKeyStatus  key_status; //!< Input status.
		NEs             notes;      //!< Notes.
		NEs::iterator   next_note;  //!< Next note.
	};

	typedef std::vector<Channel>                Channels;
	typedef std::pair  <ENoteKey, InputKeyCode> KeyBinding;
	typedef std::vector<KeyBinding>             KeyBindings;

	typedef std::shared_ptr<Chart>              ChartPtr;
	typedef std::shared_ptr<TClock>             ClockPtr;

private:
	////    Judgement and Scoring    //////////////////////////////////
	Judge           mJudge;
	RankScoreMap    mRankScores;
	unsigned int    mCombo, mMaxCombo;

	////    Chart    //////////////////////////////////////////////////
	ChartPtr        mChart;

	////    Chart data    /////////////////////////////////////////////
	CCs             mCCs;
	NEs             mNEs;


	////    Tracker state    //////////////////////////////////////////
	CCs::iterator   mNextCC;
	Channels        mChannels;

	NAs             mNAs; //!< Note audio to push to AudioManager.

	////    Clocks and Timing    //////////////////////////////////////
	ClockPtr        mClock;
	TTime const &   mTime;
	long int        mCurrentTick;

	bool            mChartEnded;
	unsigned int    mMeasureIterStart;

	////    Physical Modifiers    /////////////////////////////////////
	bool            mAutoplay;
	float           mSpeedX;

public:
	Tracker
		( ChartPtr      chart
		, Judge const & judge
		, KeyBindings   key_bindings  = {}
		, ClockPtr      ref_clock     = nullptr
		);

	ClockPtr const &  getClock() const;
	Sequence const & cgetSequence() const;
	Sequence       &  getSequence();
	NAs            &  getNAs();

	inline bool hasChartEnded() const { return mChartEnded; }

	void update();
	void updateCCs();
	void updateNEs();
};

#endif
