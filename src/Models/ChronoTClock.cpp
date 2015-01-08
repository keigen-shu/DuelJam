#include "ChronoTClock.hpp"

// Resets music time point
void TClock::resetClock (double BPM, bool startNow)
{
    tsg_tpb = 48;
    tsg_bpm = 4;
    tsg_tpm = tsg_tpb * tsg_bpm;

    tsg_tempo = 60000.0 / tsg_tpb;

    tmp_bpm  = (BPM < 0.0) ? 120.0 : BPM;
    tmp_mspt = tsg_tempo / tmp_bpm;

    isTicking = startNow;

    tct_tick = 0;
    tct_stop = 0;

    currTTime.reset();
    nextTTime.reset();

    // #HACK to allow processing of parameter events on 0:0:0.
    //
    // The clock update function only checks the interrupt AFTER ticking once in
    // the do-while loop, so the clock is not guaranteed to return at 0:0:0.
    // This causes the tracker to ignore parameter events placed on 0:0:0.
    currTTime.t = -1;

    tct_mstt = tmp_mspt;

    tpt_Music = tpt_LastRun = tpt_Segment = sysClock::now();
}

// Update clock. Returns false if interrupted.
bool TClock::update()
{
    if (isTicking) {
        const sysTimeP tpt_now = sysClock::now();

        // This casting and converting hack is done to make the clock run correctly on Linux.
        // The original works just fine on Windows.
        //       -= std::chrono::duration_cast<TimeUnit>(tpt_now - tpt_LastRun).count();
        tct_mstt -= std::chrono::duration_cast<std::chrono::microseconds>(tpt_now - tpt_LastRun).count() / 1000.0;
        tpt_LastRun = tpt_now;
    }

    while (tct_stop > 0)
    {
        if (tct_mstt <= 0) {
            tct_mstt += tmp_mspt;
            tct_stop -= 1;
        } else {
            break;
        }
    }

    while (tct_mstt <= 0)
    {
        // increment tick
        currTTime.t++;
        tct_tick++;
        tct_mstt += tmp_mspt;

        // Process beat and measure
        if (currTTime.t >= tsg_tpb) {
            currTTime.t -= tsg_tpb;
            currTTime.b++;
        }

        if (currTTime.b >= tsg_bpm) {
            currTTime.b -= tsg_bpm;
            currTTime.m++;
        }

        // Interrupted. Exit now.
        if (currTTime == nextTTime) {
            tpt_Segment = sysClock::now();

            nextTTime.reset();
            return false;
        }
    }

    return true;
}
