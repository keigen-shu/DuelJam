//  awePortAudio.cpp :: Sound output to device via PortAudio
//  Copyright 2012 - 2013 Keigen Shu

#include "awePortAudio.hpp"

#include <cmath>
#include <cstdio>

namespace awe
{

static int PaCallback(
    const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData
)
{
    APortAudio::PaCallbackPacket* data =
        (APortAudio::PaCallbackPacket*)userData;
    float* out = (float*)outputBuffer;

    /* Prevent unused argument warnings. */
    (void) inputBuffer;
    (void) timeInfo;

    if (statusFlags == paOutputUnderflow) {
        data->underflows++;
    }

    /* Library failed to update sooner. */
    if (data->output->size() < framesPerBuffer * 2) {
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            *out++ = 0;
            *out++ = 0;
        }
    } else {
        data->mutex->lock();
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            *out++ = data->output->front(); data->output->pop();
            *out++ = data->output->front(); data->output->pop();
        }
        data->mutex->unlock();
    }

    data->calls++;
    return 0;
}


bool APortAudio::init(
    unsigned int sample_rate,
    unsigned int frame_count,
    HostAPIType device_type
)
{
    mSampleRate = sample_rate;
    mFrameRate  = frame_count;

    mPAerror = Pa_Initialize();
    if (test_error()) {
        return false;
    }

    if (device_type == HostAPIType::Default) {
        mPAostream_params.device = Pa_GetDefaultOutputDevice();
    } else {
        int devices = Pa_GetDeviceCount();
        if (devices == 0) {
            mPAostream_params.device = paNoDevice;
        } else {
            for (int i = 0; i < devices; i++)
                if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(i)->hostApi)->type == static_cast<PaHostApiTypeId>(device_type)) {
                    mPAostream_params.device = i;
                    break;
                }
        }
    }

    if (mPAostream_params.device == paNoDevice) {
        fprintf(stderr, "PortAudio [error] No default output device. \n");
        Pa_Terminate();
        return false;
    }

    mPApacket.mutex       = &mOutputMutex;
    mPApacket.output      = &mOutputQueue;
    mPApacket.calls       = 0;
    mPApacket.underflows  = 0;

    mPAostream_params.channelCount = 2;  /* Stereo output. */
    mPAostream_params.sampleFormat = paFloat32;
    mPAostream_params.suggestedLatency = Pa_GetDeviceInfo(mPAostream_params.device)->defaultHighOutputLatency;
    mPAostream_params.hostApiSpecificStreamInfo = NULL;
    mPAerror = Pa_OpenStream(
                   &mPAostream, NULL,          /* One output stream, No input. */
                   &mPAostream_params,         /* Output parameters.*/
                   mSampleRate, mFrameRate,    /* Sample rate, Frames per buffer. */
                   paPrimeOutputBuffersUsingStreamCallback, PaCallback,
                   &mPApacket
               );
    if (test_error()) {
        return false;
    }

    mPAerror = Pa_StartStream(mPAostream);
    if (test_error()) {
        return false;
    }

    return true;
}

bool APortAudio::test_error() const
{
    if (mPAerror != paNoError) {
        Pa_Terminate();
        fprintf(stderr, "PortAudio [error] %d : %s \n", mPAerror, Pa_GetErrorText(mPAerror));
        return true;
    }

    return false;
}

unsigned short int APortAudio::fplay(AfBuffer const& buffer)
{
    mOutputMutex.lock();

    for (Afloat const & smp : buffer) {
        mOutputQueue.push(smp);
    }

    if (mPApacket.underflows != 0) {
        fprintf(stdout, "PortAudio [warn] %u device underflows(s) on last update.\n", mPApacket.underflows);
    }
    if (mPApacket.calls > 1) {
        fprintf(stdout, "PortAudio [warn] %u libawe underflows(s) on last update.\n", mPApacket.calls - 1);
    }

    mPApacket.calls = 0;
    mPApacket.underflows = 0;

    mOutputMutex.unlock();

    return 0;
}

void APortAudio::shutdown()
{
    mPAerror = Pa_StopStream(mPAostream);
    mPAerror = Pa_CloseStream(mPAostream);

    Pa_Terminate();

    return;
}

}
