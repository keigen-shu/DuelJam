//  InputManager.cpp :: ClanLib's input receiver
//  Copyright 2013 Keigen Shu

#include "InputManager.hpp"

KeyStatus& InputManager::getKey(const KeyCode& key) { return keys[key]; }

bool InputManager::isOff   (const KeyCode& key) const { return (keys.at(key) == KeyStatus::OFF); }
bool InputManager::isOn    (const KeyCode& key) const { return (keys.at(key) != KeyStatus::OFF); }
bool InputManager::isLocked(const KeyCode& key) const { return (keys.at(key) == KeyStatus::LOCKED); }

void InputManager::turn_on (const KeyCode& key)
{
    KeyStatus& s = keys[key];
    if (s == KeyStatus::OFF)
        s =  KeyStatus::ON;
}

void InputManager::turn_off(const KeyCode& key)
{
    keys[key] = KeyStatus::OFF;
}

bool InputManager::try_lock(const KeyCode& key)
{
    KeyStatus& s = keys[key];

    if (s == KeyStatus::ON) {
        s =  KeyStatus::LOCKED;
        return true;
    } else {
        return false;
    }
}

bool InputManager::try_unlock(const KeyCode& key)
{
    KeyStatus& s = keys[key];
    if (s == KeyStatus::LOCKED) {
        s =  KeyStatus::ON;
        return true;
    } else {
        return false;
    }
}


void InputManager::FKeyUp  (const clan::InputEvent& event) { this->turn_off(event.id); }
void InputManager::FKeyDown(const clan::InputEvent& event) { this->turn_on (event.id); }

InputManager::InputManager(clan::InputContext clIC)
{
	mCKeyUp  .connect(this, &InputManager::FKeyUp);
	mCKeyDown.connect(this, &InputManager::FKeyDown);
    clIC.get_keyboard().sig_key_up  ().connect(mCKeyUp);
    clIC.get_keyboard().sig_key_down().connect(mCKeyDown);
    // clIC.get_mouse().sig_key_up  ().connect(&CLIDCallback_KeyUp, this);
    // clIC.get_mouse().sig_key_down().connect(&CLIDCallback_KeyDown, this);

    // Create used keys in map
    try_lock(clan::keycode_escape);
    try_lock(clan::keycode_enter);
    try_lock(clan::keycode_up);
    try_lock(clan::keycode_down);
    try_lock(clan::keycode_left);
    try_lock(clan::keycode_right);
}

