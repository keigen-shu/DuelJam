//  InputManager.hpp :: ClanLib's input receiver
//  Copyright 2013 Keigen Shu

#ifndef INPUT_H
#define INPUT_H

#include <functional>
#include "__zzCore.hpp"

/**
 * A key can be in any of these four states: OFF, ON, LOCKED and AUTO.
 *
 * When the key is being pressed down, it is ON.
 * When the key is NOT being pressed down, it is OFF.
 *
 * Any function with access to this class can call try_lock() to LOCK a key
 * that has been pressed down to describe that the key has already been used by
 * the calling function. The lock is reset whenever the key is de-pressed or by
 * calling try_unlock().
 *
 * AUTO is a state used by the game to auto-trigger notes.
 */
enum class KeyStatus : uchar {
    OFF    = 0, /** When button is up.   */
    ON     = 1, /** When button is down. */
    LOCKED = 2, /** When button is down and LOCKED. */
    AUTO   = 3, /** Auto-fire */
};

/**
 * Class used to manage input from input devices.
 * TODO: Make keys more configurable.
 */
class InputManager
{
public:
    typedef int KeyCode;
    typedef std::pair<KeyCode, KeyStatus> Key;

    InputManager(clan::InputContext clIC);

    KeyStatus& getKey(const KeyCode& key);

    /** @return true if the key is not being pressed. */
    bool isOff (const KeyCode& key) const;
    /** @return true if the key is being pressed. Accounts for keys under locked status as well. */
    bool isOn (const KeyCode& key) const;
    /** @return true if the key is locked. */
    bool isLocked (const KeyCode& key) const;

    /** Sets the key status to ON. Does not do anything if key is locked. */
    void turn_on (const KeyCode& key);
    /** Sets the key status to OFF and resets the lock. */
    void turn_off (const KeyCode& key);

    /** Returns true on success or false if the key is either OFF or already locked. */
    bool try_lock (const KeyCode& key);
    /** Returns true on success or false if the key was not previously locked. */
    bool try_unlock (const KeyCode& key);

private:
    std::map<KeyCode, KeyStatus> keys;

    clan::Signal<void(const clan::InputEvent &)> mCKeyUp;
    clan::Signal<void(const clan::InputEvent &)> mCKeyDown;

    void FKeyUp  (const clan::InputEvent &);
    void FKeyDown(const clan::InputEvent &);
};
#endif
