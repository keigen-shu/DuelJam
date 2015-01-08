//  Models/NoteAudio.h :: Note audio message structure.
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

//  TODO : Implement tracker-style notes

#ifndef MODEL_NOTE_AUDIO_H
#define MODEL_NOTE_AUDIO_H

#include <cmath>

struct NoteAudio
{
    unsigned short  sampleID;
    unsigned char    trackID;

    float   volume;     //!< Cannot be NaN
    float   panning;    //!< Cannot be NaN
};

inline NoteAudio makeEmpty()
{
    return NoteAudio {
        . sampleID = 0,
        .  trackID = 0,
        . volume   = NAN,
        .panning   = NAN
    };
}

inline bool isEmpty(NoteAudio const& note)
{
    return isnan(note.volume) || isnan(note.panning);
}

#endif
