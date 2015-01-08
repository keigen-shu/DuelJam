//  AudioManager.hpp :: Game audio management system
//  Copyright 2011 - 2014 Keigen Shu

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

#include "Models/NoteAudio.h"
#include "AudioVoice.hpp"

#include "libawe/Engine.hpp"
#include "libawe/Loop.hpp"

#include "__zzCore.hpp"


using     VoiceList = std::list<     Voice >;
using NoteAudioList = std::list< NoteAudio >;

using SampleMap = std::map< unsigned int , Sample >;
using  TrackMap = std::map< unsigned char, Track* >;

/**
 * Class managing the sequencing of sound for the game.
 *
 * Each chart has a sample map which is loaded and then swapped onto
 * this class. When a play function is called, a sample-to-track map
 * node is created which would be used by the renderer when the
 * system buffer has been depleted.
 */
class AudioManager : public awe::AEngine
{
private:
    std::mutex                  mMutex;         //!< Sample and thread map mutex
    std::vector< std::thread* > mThreads;       //!< Threads relying on this.
    unsigned long               mUpdateCount;   //!< Update sync counter
    std::atomic_flag            mRunning;       //!< Thread continuation flag

    SampleMap       mSampleMap; //!< Maps a Chart specific sample ID to it's sample object.
    TrackMap        mTrackMap;  //!< Maps an ID to a track.
    VoiceList       mVoiceList; //!< List of voices to render.

public:
    /**
     * Creates and initializes the game's audio system.
     */
    AudioManager(size_t frame_count = 4096, size_t sample_rate = 48000);
    virtual ~AudioManager();
    virtual bool update();

    inline unsigned long     getUpdateCount() const { return  mUpdateCount; }
    inline std::mutex      & getMutex      ()       { return  mMutex; }
    inline std::atomic_flag& getRunning    ()       { return  mRunning; }

    inline TrackMap        * getTrackMap   ()       { return &mTrackMap; }

    void wipe_SampleMap(bool drop_data = true);
    void swap_SampleMap(SampleMap& new_map);

    bool   play(NoteAudio     const&);
    size_t play(NoteAudioList const&);

    static bool process_voice(Voice& v);

    void attach_thread(std::thread* thread_ptr);
};


#endif
