//  Engine.hpp :: Core sound engine
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_ENGINE_H
#define AWE_ENGINE_H

#include "Sources/Track.hpp"
#include "awePortAudio.hpp"

namespace awe {

/*! Master audio output interface.
 *  This class manages the output of audio from libawe into the sound
 *  device via PortAudio.
 *
 *  See the \ref EngineManual for more details about this class.
 */
class AEngine
{
protected:
    APortAudio      mOutputDevice;  //!< PortAudio output device wrapper
    Source::Track   mMasterTrack;   //!< Master output track

public:
    AEngine(
        size_t sampling_rate = 48000,
        size_t op_frame_rate = 4096,
        APortAudio::HostAPIType device_type = APortAudio::HostAPIType::Default
    ) : mOutputDevice(),
        mMasterTrack (sampling_rate, op_frame_rate, "Output to Device")
    {
        if (mOutputDevice.init(sampling_rate, op_frame_rate, device_type) == false)
            throw std::runtime_error("libawe [exception] Could not initialize output device.");
    }

    /*! Audio engine destructor.
     *  Shuts down the active PortAudio session.
     */
    virtual ~AEngine() { mOutputDevice.shutdown(); }

    /*! Retrieves the master output track which the audio engine buffers
     *  data from and then passes it into the audio output host.
     *  \return a reference to the master track object.
     */
    inline Source::Track& getMasterTrack() { return mMasterTrack; }

    /*! Pulls audio mix from master track and pushes them into the
     *  output device.
     *
     *  This operation is done only if the buffer in the output device
     *  does not have enough data for when the system demands them.
     *
     *  This call may take a very long time to complete depending on the
     *  amount of work required to pull audio data into the master track
     *  and then mix them.
     *
     *  \return false if the output device buffer has sufficient data
     *          for the next time the system requests for them.
     */
    virtual bool update()
    {
        if (mOutputDevice.getFIFOBuffer().size() < mMasterTrack.getOutput().size())
        {
            // Process stuff
            mMasterTrack.pull();
            mMasterTrack.flip();

            // Push to output device buffer
            mOutputDevice.getFIFOBuffer_mutex().lock();
            mMasterTrack.push(mOutputDevice.getFIFOBuffer());
            mOutputDevice.getFIFOBuffer_mutex().unlock();

            return true;
        } else {
            return false;
        }
    }
};

}

#endif
