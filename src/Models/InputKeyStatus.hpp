//  InputKeyStatus.hpp :: Input key status enumerator.
//  Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_INPUT_KEY_STATUS_H
#define MODEL_INPUT_KEY_STATUS_H

/*! A key can be in any of these four states: OFF, ON, LOCKED and AUTO.
 *
 *  When the key is being pressed down, it is ON.
 *  When the key is NOT being pressed down, it is OFF.
 *
 *  Any function with access to this class can call try_lock() to LOCK a key
 *  that has been pressed down to describe that the key has already been used by
 *  the calling function. The lock is reset whenever the key is de-pressed or by
 *  calling try_unlock().
 *
 *  AUTO is a state used by the game to auto-trigger notes.
 */
enum class InputKeyStatus : unsigned char {
	OFF    = 0, //!< When button is up.
	ON     = 1, //!< When button is down.
	LOCKED = 2, //!< When button is down and LOCKED.
	AUTO   = 3, //!< Auto-fire.
};

inline bool isOff   (const InputKeyStatus &s) { return s == InputKeyStatus::OFF; }
inline bool isOn    (const InputKeyStatus &s) { return s != InputKeyStatus::OFF; }
inline bool isLocked(const InputKeyStatus &s) { return s == InputKeyStatus::LOCKED; }

inline void turn_on   (InputKeyStatus &s) { if (s == InputKeyStatus::OFF) s = InputKeyStatus::ON; }
inline void turn_off  (InputKeyStatus &s) { s = InputKeyStatus::OFF; }
inline bool try_lock  (InputKeyStatus &s) { if (s == InputKeyStatus::ON) { s = InputKeyStatus::LOCKED; return true; } else { return false; } }
inline bool try_unlock(InputKeyStatus &s) { if (s == InputKeyStatus::LOCKED) { s = InputKeyStatus::ON; return true; } else { return false; } }

#endif
