//  JudgeScore.hpp :: Judgement score model
//  Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_JUDGE_SCORE_H
#define MODEL_JUDGE_SCORE_H

/** Judge rankings */
enum class EJudgeRank : unsigned char
{
	NONE    = '0',
	MISS    = 'M',
	BAD     = 'B',
	GOOD    = 'G',
	COOL    = 'C',
	PERFECT = 'P',
	AUTO    = 'A'
};

/** Judge score structure */
struct JudgeScore
{
	EJudgeRank  rank;   //!< Accuracy ranking
	int         score;  //!< Score given
	long        delta;  //!< Tick difference

	constexpr JudgeScore()
		: rank(EJudgeRank::NONE), score(0), delta(0) { }

	constexpr JudgeScore(EJudgeRank const &R, int const &S, long const &D)
		: rank(R), score(S), delta(D) { }
};

#endif

