#include "AudioManager.hpp"
#include "libawe/Filters/Mixer.hpp"
#include <chrono>
#include <list>

#if !( defined(_WIN32) || defined(_WIN64) )
#include <pthread.h> // POSIX Thread naming
#endif

AudioManager::AudioManager(size_t frame_count, size_t sample_rate)
    : awe::AEngine(sample_rate, frame_count, awe::APortAudio::HostAPIType::Default)
    , mUpdateCount(0)
    // , mRunning(ATOMIC_FLAG_INIT)
{
    mTrackMap.insert( {
        { 0, new Track(sample_rate, frame_count, "Autoplay") },
        { 1, new Track(sample_rate, frame_count, "Player 1") },
        { 2, new Track(sample_rate, frame_count, "Player 2") }
    });

    mMasterTrack.attach_source(mTrackMap[0]);
    mMasterTrack.attach_source(mTrackMap[1]);
    mMasterTrack.attach_source(mTrackMap[2]);

    mRunning.test_and_set();

    mThreads.push_back(
    new std::thread([this]() {
#if !( defined(_WIN32) || defined(_WIN64) )
        pthread_setname_np(pthread_self(), "Audio Engine");
#endif
        while (mRunning.test_and_set()) {
            if (this->update() == false) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } else {
                mUpdateCount += 1;
            }
        }
        mRunning.clear();
    })
    );
}

AudioManager::~AudioManager()
{
    mRunning.clear();
    while (mThreads.empty() == false) {
        auto it = mThreads.begin();
        (*it)->join();
        mThreads.erase(it);
    }

    wipe_SampleMap(true);
}

void AudioManager::wipe_SampleMap(bool drop_data)
{
    std::lock_guard<std::mutex> lock(mMutex);

    //  Swap collections
    VoiceList*  pVoiceList = new VoiceList();
    SampleMap*  pSampleMap = new SampleMap();

    pVoiceList->swap(mVoiceList);
    pSampleMap->swap(mSampleMap);

    //  Initialize garbage collector thread
    std::thread gc([](VoiceList * vl, SampleMap * sm, bool drop) {
        while (vl->empty() == false) {
            vl->erase(vl->begin());
        }

        while (sm->empty() == false) {
            SampleMap::iterator it = sm->begin();
            if (drop) {
                it->second.drop();
            }

            sm->erase(it);
        }

        delete vl;
        delete sm;
    },  pVoiceList, pSampleMap, drop_data
                  );
    gc.detach();
}

void AudioManager::swap_SampleMap(SampleMap& new_map)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mSampleMap.swap(new_map);
}

bool AudioManager::play(NoteAudio const& note)
{
    std::lock_guard<std::mutex> lock(mMutex);

    TrackMap ::iterator T = mTrackMap .find(note.trackID);
    if (T == mTrackMap .end()) {
        return false;
    }
    SampleMap::iterator S = mSampleMap.find(note.sampleID);
    if (S == mSampleMap.end()) {
        return false;
    }

    mVoiceList.push_back(Voice {
        & (S->second), T->second,
        awe::Filter::xSinCos(note.volume, note.panning)
    });

    return true;
}

size_t AudioManager::play(NoteAudioList const& notes)
{
    std::lock_guard<std::mutex> lock(mMutex);

    size_t count = 0;

    for(NoteAudio const & note : notes) {
        TrackMap ::iterator T = mTrackMap .find(note.trackID);
        if (T == mTrackMap .end()) {
            continue;
        }
        SampleMap::iterator S = mSampleMap.find(note.sampleID);
        if (S == mSampleMap.end()) {
            continue;
        }

        mVoiceList.remove_if(
				[=](Voice const &v) -> bool {
					return v.sample == &(S->second);
				});

        mVoiceList.push_back(Voice {
            & (S->second), T->second,
            awe::Filter::xSinCos(note.volume, note.panning)
        });

        count += 1;
    }

    return count;
}


bool AudioManager::update()
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mOutputDevice.getFIFOBuffer().size() > mMasterTrack.getOutput().size()) {
        return false;
    }

    //  Pull data from sample
    for (Voice & v : mVoiceList) {
        v.track->pull(&v);
    }

    mVoiceList.remove_if([](Voice const & v) -> bool { return !v.is_active(); });

    //  Pull data from tracks
    mMasterTrack.pull();
    mMasterTrack.flip();

    //  Push to output device buffer
    mOutputDevice.getFIFOBuffer_mutex().lock();
    mMasterTrack.push(mOutputDevice.getFIFOBuffer());
    mOutputDevice.getFIFOBuffer_mutex().unlock();

    return true;
}

void AudioManager::attach_thread(std::thread* thread_ptr)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mThreads.push_back(thread_ptr);
}
