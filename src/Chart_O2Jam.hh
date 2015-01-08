//  Chart_O2Jam.hh :: O2Jam chart reader definitions
//  Copyright 2011 - 2013 Keigen Shu

#ifndef CHART_O2JAM_HH
#define CHART_O2JAM_HH

#include <cstdint>

namespace O2Jam {

static const uint32_t OJN_SIGNATURE = 0x006E6A6F;
static const uint32_t OJM_SIGNATURE = 0x004D4A4F;  // OJM signature, "OJM\0" in little endian
static const uint32_t OMC_SIGNATURE = 0x00434D4F;  // OMC signature, "OMC\0" in little endian
static const uint32_t M30_SIGNATURE = 0x0030334D;  // M30 signature, "M30\0" in little endian

// XOR masks used in the M30 format
static const uint8_t M30_0412_XORMASK[4] = {0x30, 0x34, 0x31, 0x32}; // codec 32
static const uint8_t M30_nami_XORMASK[4] = {0x6E, 0x61, 0x6D, 0x69}; // codec 16

// Initial masking key used in the OMC-WAV format
static const uint8_t OMC_ACCKEY_INIT = 0xFF;


// arrangement table dumped from O2Jam NoteTool
static const uint8_t c_Arrangement[] = {
0x10, 0x0E, 0x02, 0x09, 0x04, 0x00, 0x07, 0x01,
0x06, 0x08, 0x0F, 0x0A, 0x05, 0x0C, 0x03, 0x0D,
0x0B, 0x07, 0x02, 0x0A, 0x0B, 0x03, 0x05, 0x0D,
0x08, 0x04, 0x00, 0x0C, 0x06, 0x0F, 0x0E, 0x10,
0x01, 0x09, 0x0C, 0x0D, 0x03, 0x00, 0x06, 0x09,
0x0A, 0x01, 0x07, 0x08, 0x10, 0x02, 0x0B, 0x0E,
0x04, 0x0F, 0x05, 0x08, 0x03, 0x04, 0x0D, 0x06,
0x05, 0x0B, 0x10, 0x02, 0x0C, 0x07, 0x09, 0x0A,
0x0F, 0x0E, 0x00, 0x01, 0x0F, 0x02, 0x0C, 0x0D,
0x00, 0x04, 0x01, 0x05, 0x07, 0x03, 0x09, 0x10,
0x06, 0x0B, 0x0A, 0x08, 0x0E, 0x00, 0x04, 0x0B,
0x10, 0x0F, 0x0D, 0x0C, 0x06, 0x05, 0x07, 0x01,
0x02, 0x03, 0x08, 0x09, 0x0A, 0x0E, 0x03, 0x10,
0x08, 0x07, 0x06, 0x09, 0x0E, 0x0D, 0x00, 0x0A,
0x0B, 0x04, 0x05, 0x0C, 0x02, 0x01, 0x0F, 0x04,
0x0E, 0x10, 0x0F, 0x05, 0x08, 0x07, 0x0B, 0x00,
0x01, 0x06, 0x02, 0x0C, 0x09, 0x03, 0x0A, 0x0D,
0x06, 0x0D, 0x0E, 0x07, 0x10, 0x0A, 0x0B, 0x00,
0x01, 0x0C, 0x0F, 0x02, 0x03, 0x08, 0x09, 0x04,
0x05, 0x0A, 0x0C, 0x00, 0x08, 0x09, 0x0D, 0x03,
0x04, 0x05, 0x10, 0x0E, 0x0F, 0x01, 0x02, 0x0B,
0x06, 0x07, 0x05, 0x06, 0x0C, 0x04, 0x0D, 0x0F,
0x07, 0x0E, 0x08, 0x01, 0x09, 0x02, 0x10, 0x0A,
0x0B, 0x00, 0x03, 0x0B, 0x0F, 0x04, 0x0E, 0x03,
0x01, 0x00, 0x02, 0x0D, 0x0C, 0x06, 0x07, 0x05,
0x10, 0x09, 0x08, 0x0A, 0x03, 0x02, 0x01, 0x00,
0x04, 0x0C, 0x0D, 0x0B, 0x10, 0x05, 0x06, 0x0F,
0x0E, 0x07, 0x09, 0x0A, 0x08, 0x09, 0x0A, 0x00,
0x07, 0x08, 0x06, 0x10, 0x03, 0x04, 0x01, 0x02,
0x05, 0x0B, 0x0E, 0x0F, 0x0D, 0x0C, 0x0A, 0x06,
0x09, 0x0C, 0x0B, 0x10, 0x07, 0x08, 0x00, 0x0F,
0x03, 0x01, 0x02, 0x05, 0x0D, 0x0E, 0x04, 0x0D,
0x00, 0x01, 0x0E, 0x02, 0x03, 0x08, 0x0B, 0x07,
0x0C, 0x09, 0x05, 0x0A, 0x0F, 0x04, 0x06, 0x10,
0x01, 0x0E, 0x02, 0x03, 0x0D, 0x0B, 0x07, 0x00,
0x08, 0x0C, 0x09, 0x06, 0x0F, 0x10, 0x05, 0x0A,
0x04, 0x00
};

//  OJN file header
struct OJN_Header {
    uint32_t  newSongID         ;   // OJN music ID
    uint32_t  Signature         ;   // OJN identifier
    float     newEncVer         ;   // OJN encoder version
    uint32_t  newGenreCode      ;   // Music genre code
    float     Tempo             ;   // Starting tempo of the song, in BPM
    uint16_t  Level         [4] ;   // 4th level is byte-aslignment padding
    uint32_t  numEvents     [3] ;
    uint32_t  numNotes      [3] ;
    uint32_t  numMeasures   [3] ;
    uint32_t  numNoteSets   [3] ;
    uint16_t  oldEncVer         ;   // [old] OJN encoder version
    uint16_t  oldSongID         ;   // [old] OJN music ID
    char      oldGenre      [20];   // [old] Music genre string
    uint32_t  oldCoverArtSize   ;   // [old] BMP music cover size
    float     oldFileVer        ;   // [old] OJN file version (for updates by notecharters)
    char      Title         [64];
    char      Artist        [32];
    char      Charter       [32];
    char      OJMFile       [32];   // OJM file name
    uint32_t  newCoverArtSize   ;   // JPEG music cover
    uint32_t  Duration      [3] ;   // Length of chart, in seconds
    uint32_t  DataOffset    [4] ;   // NoteOffset [3] + newCoverArtOffset [1]
};

struct OJN_NoteSet_Header {
    uint32_t  Measure;
    uint16_t  Channel;
    uint16_t  numEvents;
};

struct OJN_Note {
    uint16_t  SampleID;
    uint8_t   VolPan;
    uint8_t   NoteType;
};



//  OJM file headers
struct M30_File_Header {
    uint32_t    signature   ;// OJM type signature :: "M30"
    uint32_t    version     ;// M30 version
    uint32_t    encryption  ;// M30 encryption method
    uint32_t    samples     ;// Number of sounds
    uint32_t    payload_addr;// Adress of first sound data (28)
    uint32_t    payload_size;// Size of sound package (excludes this header)
    uint32_t    _padding    ;// Unknown; always 0x0000. Probably just byte padding
};

struct M30_Sample_Header {
    char        name    [32];// Sample name.
    uint32_t    size        ;// Sample size.
    uint16_t    type        ;// Sample type: 0 = M###: name _may_ contain  file extension;
                             //              5 = W###: name never contains file extension
    uint16_t    _unknown0002;// Unknown; always 0x0002
    uint32_t    _unknowntype;// Unknown; always 0x0000001 if type == 0;
                             //                 0x8000000 if type == 5.
    uint16_t    id          ;// Sample reference ID (used by notes)
    uint16_t    _padding    ;// Unknown; always 0x0000. Probably just byte padding
    uint32_t    _pcm_samples;// Number of PCM samples in OGG file [unused]
};

struct OMC_File_Header {
    uint32_t    signature   ;// "OMC" or "OJM"
    uint16_t    wavs        ;// Number of WAV files
    uint16_t    oggs        ;// Number of OGG files
    uint32_t    wavs_addr   ;// Location of first WAV file
    uint32_t    oggs_addr   ;// Location of first OGG file
    uint32_t    file_size   ;// OJM/OMC file size (including header)
};

struct OMC_WAV_Header {
    char        name    [32]    ;// Sample name; always without extension
    uint16_t    fmt0_AudioFormat;// WAVE codec
    uint16_t    fmt0_numChannels;// Number of sample channels
    uint32_t    fmt0_SmplRate   ;// Samples per second (8000, 44100, etc.)
    uint32_t    fmt0_ByteRate   ;//   bytes per second (SampleRate * PlayRate)
    uint16_t    fmt0_PlayRate   ;//   bytes per sample frame [NumChannels * BitRate / 8]
    uint16_t    fmt0_BitRate    ;//    bits per sample (8, 16 or 32-bit)
    uint32_t    data_ChunkID    ;// Data header -- "data" // empty
    uint32_t    data_ChunkSize  ;// Size of actual sample data
};

struct OMC_OGG_Header {
    char        name[32];// Sample name; always with extension
    uint32_t    size    ;// Sample file size
};

// WAV header for reconstruction
struct WAV_Header {
    uint32_t RIFF_ID;
    uint32_t RIFF_Size;
    uint32_t RIFF_fmt0;
    uint32_t fmt0_ID;
    uint32_t fmt0_Size;
    uint16_t fmt0_AudioFormat;
    uint16_t fmt0_numChannels;
    uint32_t fmt0_SmplRate;
    uint32_t fmt0_ByteRate;
    uint16_t fmt0_PlayRate;
    uint16_t fmt0_BitRate;
    uint32_t data_ChunkID;
    uint32_t data_ChunkSize;
};

}

#endif
