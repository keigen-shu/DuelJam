//  UI/Common.hpp :: Common UI Code
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef UI_COMMON_H
#define UI_COMMON_H

#include "../__zzCore.hpp"

namespace UI        {
namespace Common    {

enum Switch : uint8_t {
    OFF         = 0x0,
    ON          = 0x1,

    __STATE     = 0x1,
    __CHANGING  = 0x2
};

//!@name Global Methods and Operator Overrides for Switch
//!@{

inline std::uint8_t operator* (const Switch& sw) { return static_cast<std::uint8_t>(sw); }
inline std::uint8_t operator* (Switch&& sw) { return static_cast<std::uint8_t>(sw); }

inline bool isOn  (const Switch &sw) { return (*sw & *Switch::__STATE) == *Switch::__STATE; }
inline bool isOff (const Switch &sw) { return !isOn(sw); }

inline bool isChanging (const Switch &sw) { return (*sw & *Switch::__CHANGING) == *Switch::__CHANGING; }

inline void toggleChanging (Switch &sw) { sw = static_cast<Switch>(*sw ^  *Switch::__CHANGING); }
inline void    setChanging (Switch &sw) { sw = static_cast<Switch>(*sw |  *Switch::__CHANGING); }
inline void  resetChanging (Switch &sw) { sw = static_cast<Switch>(*sw & ~*Switch::__CHANGING); }

inline Switch operator& (const Switch &a, const Switch &b) { return static_cast<Switch>(*a & *b); } //!< Bit-wise AND operator
inline Switch operator| (const Switch &a, const Switch &b) { return static_cast<Switch>(*a | *b); } //!< Bit-wise  OR operator
inline Switch operator^ (const Switch &a, const Switch &b) { return static_cast<Switch>(*a ^ *b); } //!< Bit-wise XOR operator

//! Inverts the state
inline Switch operator~ (const Switch &sw) { return static_cast<Switch>(*sw ^  *Switch::__STATE); }

//! Forces the state to ON
inline Switch operator+ (const Switch &sw) { return static_cast<Switch>(*sw |  *Switch::__STATE); /* b'00000001' */ }

//! Forces the state to OFF
inline Switch operator- (const Switch &sw) { return static_cast<Switch>(*sw & ~*Switch::__STATE); /* b'11111110' */ }

//!@}


}
}
#endif
