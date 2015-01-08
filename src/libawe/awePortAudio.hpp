//  awePortAudio.hpp :: Sound output to device via PortAudio
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_PORTAUDIO_H
#define AWE_PORTAUDIO_H

#include <portaudio.h>
#include <mutex>
#include "Define.hpp"

namespace awe {

/*! PortAudio class to handle communication with the audio device.
 */
class APortAudio
{
public:
    //! PortAudio callback data structure.
    struct PaCallbackPacket
    {
        std::mutex  *   mutex;      //<! Output FIFO buffer mutex.
        AfFIFOBuffer*   output;     //<! Output FIFO buffer pointer.
        unsigned char   calls;      //<! Number of times PA ran this callback since last update.
        unsigned char   underflows; //<! Number of times PA reported underflow problems since last update.
    };

    //! PortAudio audio output host API enumerator
    enum class HostAPIType : int {
        Default = 0, // paInDevelopment
        DS      = paDirectSound,
        MME     = paMME,
        ASIO    = paASIO,
        SM      = paSoundManager,
        CA      = paCoreAudio,
        OSS     = paOSS,
        ALSA    = paALSA,
        AL      = paAL,
        BeOS    = paBeOS,
        WDMKS   = paWDMKS,
        JACK    = paJACK,
        WASAPI  = paWASAPI,
        ASHPI   = paAudioScienceHPI
    };

private:
    PaError             mPAerror;
    PaStream          * mPAostream;
    PaStreamParameters  mPAostream_params;
    PaCallbackPacket    mPApacket;

    AfFIFOBuffer        mOutputQueue;
    std::mutex          mOutputMutex;

    unsigned int    mSampleRate;
    unsigned int    mFrameRate;

    //! Checks if PortAudio has an error
    bool test_error() const;

public:
    inline unsigned char pa_calls           () const { return mPApacket.calls; }
    inline double        pa_stream_cpu_load () const { return Pa_GetStreamCpuLoad(mPAostream); }
    inline double        pa_stream_time     () const { return Pa_GetStreamTime   (mPAostream); }
    inline AfFIFOBuffer& getFIFOBuffer      ()       { return mOutputQueue; }
    inline std::mutex  & getFIFOBuffer_mutex()       { return mOutputMutex; }

    inline unsigned int  getSampleRate() const { return mSampleRate; }
    inline unsigned int  getFrameRate () const { return mFrameRate ; }

    //! Plays provided buffer. @returns underruns since last play.
    unsigned short int fplay(const AfBuffer& buffer);

    bool init(
            unsigned int sample_rate,
            unsigned int frame_count,
            HostAPIType device_type = HostAPIType::Default
            );
    void shutdown();
};
}
#endif
