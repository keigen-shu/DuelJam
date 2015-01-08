//  AudioVoice.hpp
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AUDIO_VOICE_H
#define AUDIO_VOICE_H

#include <memory>

#include "libawe/Frame.hpp"
#include "libawe/Source.hpp"
#include "libawe/Sample.hpp"
#include "libawe/Sources/Track.hpp"

using Sample = awe::Asample;
using  Track = awe::Source::Track;

struct SoXR;

class Voice : public awe::Asource
{
public:
	Sample*         sample;
	Track *         track;
	awe::Asfloatf   chanGain;

private:
	std::shared_ptr<SoXR> soxr;

public:
	Voice(Sample* _sample, Track* _track, awe::Asfloatf _gain);
	virtual ~Voice();
	virtual void drop();
	virtual void make_active(void*);
	virtual bool is_active() const;
	virtual void render(awe::AfBuffer& buffer, const awe::ArenderConfig& config);
};

#endif