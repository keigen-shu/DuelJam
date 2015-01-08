//  Sources/Track.hpp :: Sound mixing track
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_SOURCE_TRACK_H
#define AWE_SOURCE_TRACK_H

#include <mutex>
#include <string>
#include "../Define.hpp"
#include "../Source.hpp"
#include "../Filters/Rack.hpp"

namespace awe {
namespace Source {

/*! Sound mixer class.
 *
 *  This class is used to manage and mix multiple sound sources into one
 *  output track. Filters can be chained to the filter rack to apply
 *  post-mixing sound effects.
 *
 *  All tracks are double-buffered; the internal inaccessible source
 *  mixing pool is labelled P while the output pool is labelled O.
 *
 *  Every track has two mutexes; one is used to lock the pool buffer,
 *  source list and pool config and the other is used to to lock the
 *  output buffer and filter rack.
 */
class Track : public Asource
{
    using MutexLockGuard = std::lock_guard< std::mutex >;
    using AscRack        = Filter::Rack<2>;

private:
    mutable std::mutex  mPmutex;    //!< Track pool mutex
    mutable std::mutex  mOmutex;    //!< Track output mutex

    std::string         mName;      //!< Track label (for identifying tracks)
    ArenderConfig       mPconfig;   //!< Track render configuration

    AsourceSet  mPsources;  //!< Sound sources to mix from
    AfBuffer    mPbuffer;   //!< Mixing buffer
    AfBuffer    mObuffer;   //!< Output buffer
    AscRack     mOfilter;   //!< Post-mixing filter rack

    bool        mqActive;   //!< Is this source active?

private:
    //!\name Non-thread-safe methods
    //!\{

    //! Pull source into pool buffer, without mutex lock.
    void fpull(Asource* src);

    //! Pull assigned sources into pool buffer, without mutex lock.
    void fpull();

    //! Flip pool buffer with output buffer, without mutex lock.
    void fflip();

    //! Apply filter rack onto output buffer, without mutex lock.
    void ffilter();

    //!\}

public:
    Track(size_t sample_rate, size_t frames, std::string name = "Unnamed Track");

    /*! This call does nothing on a track object.
     *  \warning This call does not drop any of the source and filter
     *           objects referenced by this class. Please track these
     *           objects on your own to prevent memory leaks.
     */
    virtual void drop() override { }

    /*! Activates this track this track if there are some sources in
     *  the audio source pool.
     */
    virtual void make_active(void*) override
    {
        MutexLockGuard p_lock(mPmutex);
        mqActive = !mPsources.empty();
    }

    /*! Queries whether or not this track is up and running.
     *  \return true if track is activated and running.
     */
    virtual bool is_active() const override
    {
        MutexLockGuard p_lock(mPmutex);
        return mqActive;
    }

    virtual void render(AfBuffer &targetBuffer, const ArenderConfig &targetConfig) override;

    /*! Retrieves the source pool renderer configuration structure of
     *  this track.
     *  \return a read-only reference to the current configuration structure.
     */
    inline const ArenderConfig& getConfig() const { return mPconfig; }

    /*! Sets the source pool renderer configuration structure of this
     *  track.
     *  \param new_config the new configuration to use in this track.
     */
    inline void setConfig(const ArenderConfig &new_config)
    {
        MutexLockGuard p_lock(mPmutex);
        mPconfig = new_config;
    }

    /*! Retrieves the output mutex object which controls the output
     *  buffer and the rack.
     *  \return a reference to the output mutex of this track.
     */
    inline std::mutex & getMutex() { return mOmutex; }


    /*! Retrieves the name (\ref mName) of this track.
     *  \return a read-only reference to the name string.
     */
    inline std::string const & getName() { return mName; }

    /*! Retrieves the source list that this track buffers data from.
     *  \return a read-only reference to the source list of this track.
     */
    inline const AsourceSet& getSources() const { return mPsources; }

    /*! Retrieves the track output buffer.
     *  \warning Ownership of this object is defined by the output
     *           mutex obtainable through the \ref getMutex() call.
     *  \return a reference to the output buffer of this track.
     */
    inline const AfBuffer  & getOutput () const { return mObuffer; }

    /*! Retrieves the track filter rack.
     *  \warning Ownership of this object is defined by the output
     *           mutex obtainable through the \ref getMutex() call.
     *  \return a reference to the filter rack of this track.
     */
    inline AscRack& getRack() { return mOfilter; }

    /*! Counts the number of active sources within the source pool.
     *  \return number of active sources within the pooling list
     */
    inline size_t count_active_sources() const
    {
        MutexLockGuard p_lock(mPmutex);
        size_t count = 0;
        for(Asource const * src : mPsources)
        {
            if (src->is_active() == true)
                count++;
        }
        return count;
    }

    /*! Counts the number of sources assigned to the source pool.
     *  \return number of sources within the pooling list
     */
    inline size_t count_sources() const {
        MutexLockGuard p_lock(mPmutex);
        return mPsources.size();
    }

    /*! Inserts a source into the pooling list.
     *  \param src[in] pointer to the source object to be inserted.
     */
    inline void attach_source(Asource* const src)
    {
        MutexLockGuard p_lock(mPmutex);
        if (mPsources.count(src) == 0)
            mPsources.insert(src);

        mqActive = true;
    }

    /*! Removes a source from the pooling list.
     *  \return false if the no objects were deleted from the pooling
     *          list.
     */
    inline bool detach_source(Asource* const src)
    {
        MutexLockGuard p_lock(mPmutex);
        bool r = mPsources.erase(src) != 0;
        mqActive = !mPsources.empty();
        return r;
    }

    //! Pull assigned sources into pool buffer, with mutex lock.
    inline void pull()
    {
        MutexLockGuard p_lock(mPmutex);
        fpull();
    }

    //! Pulls the sources pool buffer, with mutex lock.
    inline void pull(Asource *src)
    {
        MutexLockGuard p_lock(mPmutex);
        fpull(src);
    }

    //! Flip pool buffer with output buffer, with mutex lock.
    inline void flip()
    {
        // Lock both mutexes without deadlock.
        std::lock(mPmutex, mOmutex);

        MutexLockGuard o_lock(mOmutex, std::adopt_lock);
        {
            // Unlock pool mutex after flipping.
            MutexLockGuard p_lock(mPmutex, std::adopt_lock);
            fflip();
        }

        ffilter();
    }

    /*! Pushes the output buffer into a queue buffer.
     *  \param queue[out] FIFO buffer to write the output buffer to
     */
    inline void push(AfFIFOBuffer &queue) const
    {
        MutexLockGuard o_lock(mOmutex);

        for(auto s : mObuffer)
            queue.push(s);
    }

};


}
}

#endif
