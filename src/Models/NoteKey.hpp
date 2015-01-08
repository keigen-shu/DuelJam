//  Models/NoteKey.hpp :: Note event key enumerator
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef MODEL_NOTE_KEY_H
#define MODEL_NOTE_KEY_H

#include <cstdint>

enum class ENoteKey : uint8_t
{
	_player_mask_ = 0xF0,

	P1_S = 0x10, P1_1 = 0x11, P1_2 = 0x12, P1_3 = 0x13,
	P1_4 = 0x14, P1_5 = 0x15, P1_6 = 0x16, P1_7 = 0x17,
	P1_8 = 0x18, P1_9 = 0x19,

	P2_S = 0x20, P2_1 = 0x21, P2_2 = 0x22, P2_3 = 0x23,
	P2_4 = 0x24, P2_5 = 0x25, P2_6 = 0x26, P2_7 = 0x27,
	P2_8 = 0x28, P2_9 = 0x29,

	AUTO = 0x30, BG   = 0x40
};

inline uint8_t operator*(const ENoteKey &v) { return static_cast<uint8_t>(v); }

inline ENoteKey ENoteKey_makeBG      (const uint8_t &v) { return static_cast<ENoteKey>(v % 192 + *ENoteKey::BG);   } // Wrap to BG range [0x40 .. 0xFF]
inline ENoteKey ENoteKey_makeAutoPlay(const uint8_t &v) { return static_cast<ENoteKey>(v % 16  + *ENoteKey::AUTO); } // Wrap to AP range [0x30 .. 0x3F]
inline bool ENoteKey_isBG      (const ENoteKey &v) { return *v >= *ENoteKey::BG; }
inline bool ENoteKey_isAutoPlay(const ENoteKey &v) { return *v >= *ENoteKey::AUTO; }
inline bool ENoteKey_isPlayer1 (const ENoteKey &v) { return *v >= *ENoteKey::P1_S && *v < *ENoteKey::P2_S; }
inline bool ENoteKey_isPlayer2 (const ENoteKey &v) { return *v >= *ENoteKey::P2_S && *v < *ENoteKey::AUTO; }

inline uint8_t ENoteKey_getPlayer(const ENoteKey &key) {
	if (ENoteKey_isAutoPlay(key)) return 0;
	if (ENoteKey_isPlayer1 (key)) return 1;
	if (ENoteKey_isPlayer2 (key)) return 2;

	return -1;
}

#endif

