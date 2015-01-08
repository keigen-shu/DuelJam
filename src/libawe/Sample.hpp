//  Sample.hpp :: Sound sample class
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_SAMPLE_H
#define AWE_SAMPLE_H

#include <memory>
#include "Define.hpp"

namespace awe {

class Asample {
private:
    /** Pointer to audio buffer data.
     *  This pointer can be null at creation and be assigned to later.
     */
    std::shared_ptr<AiBuffer> mSource;

    Achan           mChannels;      //!< Number of channels on the source buffer.

    /** Audio buffer data peak gain applied before being sent to mixer.
     *
     *  Many sound formats allow the storage of audio samples beyond
     *  their own dynamic range. libawe will apply a negative gain to
     *  scale the data to fit within the 16-bit integer range which is
     *  compensated for when rendering.
     */
    Afloat          mSourcePeak;

    unsigned long   mSampleRate;    //!< Sampling rate of sound sample.
    std::string     mSampleName;    //!< Descriptive name of the sample.

public:
    Asample() : mSource(nullptr), mChannels(0), mSourcePeak(1.0f), mSampleRate(0), mSampleName("null") { }

    /** Default constructor
     *
     *  \param _source Audio buffer source.
     *  \param _peak   Audio buffer peak compensation multiplier.
     *  \param _rate   Audio buffer source sample rate.
     *  \param _name   Audio buffer name. (Not used by this class)
     */
    Asample(std::shared_ptr<AiBuffer> &_source,
            const Achan         &_chan,
            const Afloat        &_peak,
            const unsigned long &_rate,
            const std::string   &_name = "Unnamed sample"
    )   : mSource       (_source)
        , mChannels     (_chan)
        , mSourcePeak   (_peak)
        , mSampleRate   (_rate)
        , mSampleName   (_name)
    { }

    /** Load from file constructor.
     *
     *  \warning This function blocks execution and leaves source as
     *           `nullptr` if it fails to load the sample from file.
     */
    Asample(const std::string &file);

    /** Load from memory constructor.
     *
     *  \warning This function blocks execution and leaves source as
     *           `nullptr` if it fails to load the sample from memory.
     */
    Asample(char* mptr, const size_t &size, const std::string &_name = "Unnamed sample");

    virtual ~Asample() { }

    inline bool drop() {
        if (mSource) {
            mSource.reset();
            return true;
        } else {
            return false;
        }
    }

    /** Assigns a audio buffer to the sample.
     *
     *  \param _source Audio buffer source.
     *  \param _peak   Audio buffer peak compensation multiplier.
     *  \param _rate   Audio buffer source sample rate.
     */
    inline void setSource(std::shared_ptr<AiBuffer> _source, Afloat _peak)
    {
        mSource     = _source;
        mSourcePeak = _peak;
    }

    inline std::shared_ptr<const AiBuffer> cgetSource() const { return mSource; }
    inline std::shared_ptr<      AiBuffer>  getSource()       { return mSource; }

    inline Achan         getChannelCount() const { return mChannels; }
    inline size_t        getFrameCount  () const { return mSource->size() / mChannels; }
    inline Afloat        getPeak        () const { return mSourcePeak; }
    inline unsigned long getSampleRate  () const { return mSampleRate; }
    inline std::string   getSampleName  () const { return mSampleName; }
    inline std::string   getName        () const { return mSampleName; }
};

}

#endif
