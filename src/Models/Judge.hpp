//  Models/Judge.hpp :: Judgement table
//  Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_JUDGE_H
#define MODEL_JUDGE_H

#include <cmath>
#include "JudgeScore.hpp"

/**
 * Scoring window calculator
 *
 * The timing window is defined in milliseconds, but this is then translated to
 * tick duration (i.e. based on music tempo) and normalized.
 */
class Judge
{
private:
    // 0 <-- Perfect --> PC <-- Cool --> CG <-- Good --> GB <-- Bad --> BM <-- Miss

    /* Ranking timing data */
    long rP, rC, rG, rB; // Real half-duration base reference
    long tP, tC, tG, tB; // Tick half-duration (after translation)

public:
    constexpr Judge(
        unsigned const &_rP, unsigned const &_rC,
        unsigned const &_rG, unsigned const &_rB
    ) : rP(_rP), rC(_rC), rG(_rG), rB(_rB),
        tP(0  ), tC(0  ), tG(0  ), tB(0  )
    { }

    inline long const &cgetTP() const { return tP; }
    inline long const &cgetTC() const { return tC; }
    inline long const &cgetTG() const { return tG; }
    inline long const &cgetTB() const { return tB; }

    /**
     * Sets the base duration-window timing in milliseconds.
     * l.b. stands for "lower boundary"
     * u.b. stands for "upper boundary"
     *
     * @param _r0P duration of timing window between tick 0 and the l.b. of PERFECT
     * @param _rPC duration window between u.b. of PERFECT and the l.b. of COOL
     * @param _rCG duration window between u.b. of COOL and the l.b. of GOOD
     * @param _rGB duration window between u.b. of GOOD and the l.b. of BAD
     * @param _rBM duration window between u.b. of BAD and the l.b. of MISS
     */
    inline void setBaseTiming(
        unsigned const &_rP, unsigned const &_rC,
        unsigned const &_rG, unsigned const &_rB
    ) {
        rP = _rP, rC = _rC, rG = _rG, rB = _rB;
    }

    /**
     * Calculates the tick-duration rank timing-window using supplied
     * milliseconds per tick value.
     *
     * @param tempo_mspt      operating tempo in milliseconds per tick.
     * @param ceiling_factor  tick rounding (up) factor. (default is 2)
     */
    inline void calculateTiming(
        const double &tempo_mspt,
        const unsigned long &ceiling_factor = 2
    ) {
        tP = lround(static_cast<double>(rP) / tempo_mspt);
        tP += ceiling_factor - tP % ceiling_factor;
        tC = lround(static_cast<double>(rC) / tempo_mspt);
        tC += ceiling_factor - tC % ceiling_factor;
        tG = lround(static_cast<double>(rG) / tempo_mspt);
        tG += ceiling_factor - tG % ceiling_factor;
        tB = lround(static_cast<double>(rB) / tempo_mspt);
        tB += ceiling_factor - tB % ceiling_factor;
    }

    inline bool isInScoringRange(const long &delta) const
    {
        return (delta < tP + tC + tG + tB);
    }

    /**
     * Judges current note.
     * @param delta tick difference
     * @return score.
     */
    inline JudgeScore judge(const long &delta) const
    {
        if (delta < 0 - tP - tC - tG - tB)
            return JudgeScore(EJudgeRank::MISS, 0, delta); /* miss; too late */

        else if (std::abs(delta) < tP)
            return JudgeScore(EJudgeRank::PERFECT, 50 + 50 * ((delta) / tP), delta);

        else if (std::abs(delta) < tP + tC)
            return JudgeScore(EJudgeRank::COOL, 25 + 25 * ((delta - tP) / tC), delta);

        else if (std::abs(delta) < tP + tC + tG)
            return JudgeScore(EJudgeRank::GOOD, 10 + 15 * ((delta - tP - tC) / tG), delta);

        else if (std::abs(delta) < tP + tC + tG + tB)
            return JudgeScore(EJudgeRank::BAD, 0 + 12 * ((delta - tP - tC - tG) / tB), delta);

        else
            /* if (delta >= tP + tC + tG + tB) */
            return JudgeScore(EJudgeRank::NONE, 0, delta); /* none; too early */

    }
};

/** Standard timing duration windows */   // PFCT, COOL, GOOD, <BAD || Duration frame count on a standard 60-Hz screen
const Judge JEasy   (125, 150, 125, 100); //  250,  550,  800, 1000 || 15.0 , 18.0 , 15.0 , 12.0  >>> max 60.0  frames
const Judge JNormal ( 40,  60,  80, 100); //   80,  200,  360,  460 ||  4.8 ,  7.2 ,  9.6 , 12.0  >>> max 33.6  frames
const Judge JHard   ( 25,  35,  40,  50); //   50,  120,  200,  300 ||  3.0 ,  4.2 ,  4.8 ,  6.0  >>> max 18.0  frames
const Judge JLunatic( 16,  18,  22,  24); //   32,   68,  112,  160 ||  1.92,  2.16,  2.64,  2.88 >>> max  9.6  frames
const Judge JJikogu (  8,  10,  10,   8); //   16,   36,   56,   72 ||  0.96,  1.2 ,  1.2 ,  0.96 >>> max  4.32 frames

const Judge JDebug  (100, 100, 100, 100);

#endif /* JUDGE_H */
