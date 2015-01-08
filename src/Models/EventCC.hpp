//  Models/EventCC.hpp :: Control change event model
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_EVENT_CC_H
#define MODEL_EVENT_CC_H

#include <cstdint>
#include "ChronoTTime.hpp"

/** Control enumerator class.
 *  The most significant bit is not reserved and may be used for other
 *  things.
 */
enum class EControl : uint16_t
{
	_mask_ = 0x8000, //!< Range mask

	////    Clock Controls (0x03xx)    ////
	CLOCK_TEMPO     = 0x0310,
	CLOCK_TS_A      = 0x0321, // Time-signature Ticks per Beat
	CLOCK_TS_B      = 0x0322, // Time-signature Beats per Measure
	CLOCK_TS_Z      = 0x0324, // Floating-point Ticks per Measure

	CLOCK_STOP_T    = 0x0325, // Tick-base stop
	CLOCK_STOP_R    = 0x0326, // Real-base stop

	////    Tracker Controls (0x0Dxx)    ////
	MOD_SPEED_X     = 0x0D10, // Speed multiplier

	////    Judgement Controls    ////


	////    Audio Engine Controls    ////
	AUDIO_VOL       = 0x0111, // Global volume
	AUDIO_PAN       = 0x0112, // Global panning

	/* Video Engine Parameters */
};

////    Operator overloads    ////
inline uint16_t operator* (EControl v) { return static_cast<uint16_t>(v); }
inline EControl toEControl(uint16_t v) { return static_cast<EControl>(v); }

inline EControl operator&(const EControl &a, const EControl &b) { return toEControl( *a & *b ); }
inline EControl operator^(const EControl &a, const EControl &b) { return toEControl( *a ^ *b ); }
inline EControl operator|(const EControl &a, const EControl &b) { return toEControl( *a | *b ); }

inline EControl operator+(const EControl &v) { return v | EControl::_mask_  ; } // Set ON
inline EControl operator-(const EControl &v) { return v & toEControl(0x7FFF); } // Set OFF

inline bool isOn (const EControl &v) { return v >= EControl::_mask_; }
inline bool isOff(const EControl &v) { return v <  EControl::_mask_; }

////    Control change event class    ////
struct EventCC
{
	union Value {
		float   asFloat;
		int32_t asInteger;

		Value(float   f) : asFloat  (f) { }
		Value(int32_t i) : asInteger(i) { }
	};

	//  Attributes
    TTime       t;
	EControl    c;
	Value       v;

	EventCC(TTime time, EControl control, float   value)
		: t(time), c(control), v(value) { }
	EventCC(TTime time, EControl control, int32_t value)
		: t(time), c(control), v(value) { }

};

inline bool cmpEventCC_Greater(const EventCC &a, const EventCC &b)
{ return (a .t == b .t) ? (a .c <  b .c) : (a .t <  b .t); }
inline bool cmpEventCC_GreaterTime(const EventCC &a, const EventCC &b)
{ return (a .t <  b .t); }

#endif
