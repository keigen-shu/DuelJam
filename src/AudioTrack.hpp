//  AudioTrack.hpp :: Game audio track with UI bindings
//  Copyright 2014 Keigen Shu

#ifndef AUDIO_TRACK_H
#define AUDIO_TRACK_H

#include "__zzCore.hpp"
#include "libawe/Sources/Track.hpp"
#include "libawe/Filters/3BEQ.hpp"
#include "libawe/Filters/Mixer.hpp"
#include "libawe/Filters/Metering.hpp"
#include "UI/Slider.hpp"
#include "UI/SwitchButton.hpp"

/* Margin 1px; Padding 2px;
 */
class AudioTrack : public clan::View
{
private:
    awe::Source::Track          *mTrack;

    awe::Filter::TBEQ<2>        *m3BEQ;
    awe::Filter::AscMixer<2>    *mMixer;
    awe::Filter::AscMetering    *mMeter;

    ////    GUI Controls    ///////////////////////////////////////////
    UI::Slider                  mGCsdvEQGainL;
    UI::Slider                  mGCsdvEQGainM;
    UI::Slider                  mGCsdvEQGainH;

    UI::Slider                  mGCsdhEQFreqL;
    UI::Slider                  mGCsdhEQFreqH;

    UI::Slider                  mGCsdvGain;
    UI::Slider                  mGCsdhPan;

    UI::SwitchButton            mGCbtnMute;
    UI::SwitchButton            mGCbtnToggleSize;

public:
    AudioTrack(
        awe::Source::Track *source
    );

    ~AudioTrack();

    static sizei _getSize(bool mini = false);

    awe::Source::Track       *  getTrack()       { return mTrack; }
    awe::Source::Track const * cgetTrack() const { return mTrack; }

    ////    GUI Component Methods    //////////////////////////////////
    void render_content(clan::Canvas &canvas) override;

    void eq_gain_changed();
    void eq_freq_changed();

    void mixer_value_changed();

    void mute_toggled(bool);
    void size_toggled(bool);
};


#endif
