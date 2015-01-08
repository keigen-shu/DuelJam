//  AudioVoice.cpp
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#include "AudioVoice.hpp"

#include <cstdio>
#include "soxr/src/soxr.h"

size_t soxr_input_fn(SoXR*, soxr_cbuf_t*, size_t);

struct SoXR {
	soxr_t          soxr;
	soxr_error_t    soxr_error;

	std::shared_ptr<awe::AiBuffer>
				iptr; //!< Input pointer
	size_t      chan; //!< Number of channels in sound sample.
	size_t      size; //!< Number frames in sound sample to play.
	size_t      read; //!< Number of frames read from input buffer.

	SoXR(Sample* sample, unsigned long output_sample_rate)
		: soxr(0)
		, soxr_error(nullptr)
		, iptr(sample->getSource())
		, chan(sample->getChannelCount())
		, size(sample->getFrameCount())
		, read(0)
	{
		soxr_io_spec_t      const soxIOs = soxr_io_spec(SOXR_INT16_I, SOXR_FLOAT32_I);
		soxr_quality_spec_t const soxQs  = soxr_quality_spec(
				// This ternary statement is a temporary workaround for a crashing bug in SoXR 0.1.1.
				// http://sourceforge.net/p/soxr/discussion/general/thread/29cfb185
				std::fabs(sample->getSampleRate() / output_sample_rate - 1) < 1e-6
				? SOXR_QQ : SOXR_MQ, 0
				);
		soxr_runtime_spec_t const soxRTs = soxr_runtime_spec(1);

		soxr = soxr_create(
				static_cast<double>  (sample->getSampleRate()),     // Input rate
				static_cast<double>  (output_sample_rate),          // Output rate
				static_cast<unsigned>(sample->getChannelCount()),   // Channel Count
				&soxr_error, &soxIOs, &soxQs, &soxRTs
				);
		if (soxr_error) { throw new std::runtime_error(soxr_error); }

		soxr_error = soxr_set_input_fn(
				soxr, (soxr_input_fn_t) soxr_input_fn,
				this, IO_BUFFER_SIZE
				);
		if (soxr_error) { throw new std::runtime_error(soxr_error); }
	}

	~SoXR() {
		if (soxr != 0)
			soxr_delete(soxr);
	}
};

size_t soxr_input_fn(SoXR* ptr, soxr_cbuf_t* buf, size_t len)
{
	*buf = (ptr->iptr->data() + (ptr->read * ptr->chan));

	/****/ if (ptr->read >= ptr->size) {
		len = 0;
	} else if (ptr->read + len >= ptr->size) {
		len = ptr->size - ptr->read;
	} else if (ptr->read + len <  ptr->size) {
		len = len;
	}

	ptr->read += len;
	return len;
}

Voice::Voice(Sample* _sample, Track* _track, awe::Asfloatf _gain)
	: sample    (_sample)
	, track     (_track )
	, chanGain  (_gain  )
	, soxr      (std::make_shared<SoXR>(sample, track->getConfig().sampleRate))
{ }

Voice::~Voice () {
	soxr.reset();
}

void Voice::drop() { }

void Voice::make_active(void*) {
	soxr = std::make_shared<SoXR>(sample, track->getConfig().sampleRate);
}

bool Voice::  is_active() const {
	return soxr->read < soxr->size;
}

void Voice::render(awe::AfBuffer& buffer, const awe::ArenderConfig& config)
{
	awe::AfBuffer oBuffer(buffer.size(), 0.f);

	switch (config.quality)
	{
	case awe::ArenderConfig::Quality::MUTE:
		soxr_output(soxr->soxr, oBuffer.data(), config.frameCount);
		soxr->soxr_error = soxr_error(soxr->soxr);
		if (soxr->soxr_error) { throw std::runtime_error(soxr->soxr_error); }

	case awe::ArenderConfig::Quality::SKIP:
		return;

	default:
		size_t oDone = soxr_output(soxr->soxr, oBuffer.data(), config.frameCount);
		soxr->soxr_error = soxr_error(soxr->soxr);
		if (soxr->soxr_error) { throw std::runtime_error(soxr->soxr_error); }

		/****/ if (sample->getChannelCount() == 2) {
			for (size_t i = 0; i < oDone; i++) {
				size_t j = config.frameOffset + i;
				buffer.data() [j*2  ] += oBuffer[i*2  ] * chanGain[0] * sample->getPeak();
				buffer.data() [j*2+1] += oBuffer[i*2+1] * chanGain[1] * sample->getPeak();
			}
		} else if (sample->getChannelCount() == 1) {
			for (size_t i = 0; i < oDone; i++) {
				size_t j = config.frameOffset + i;
				buffer.data() [j*2  ] += oBuffer[i  ] * chanGain[0] * sample->getPeak();
				buffer.data() [j*2+1] += oBuffer[i  ] * chanGain[1] * sample->getPeak();
			}
		}

		return;
	}
}
