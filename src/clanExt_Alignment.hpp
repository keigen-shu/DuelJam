//  clanExt_Alignment.hpp :: Alignment math
//  authored by Chu Chin Kuan
//
//  :: LICENSE AND COPYRIGHT ::
//
//  The author disclaims copyright to this source code.
//
//  The author or authors of this code dedicate any and all copyright interest
//  in this code to the public domain. We make this dedication for the benefit
//  of the public at large and to the detriment of our heirs and successors.
//
//  We intend this dedication to be an overt act of relinquishment in perpetuity
//  of all present and future rights to this code under copyright law.

#ifndef H_CLAN_EXT_ALIGNMENT
#define H_CLAN_EXT_ALIGNMENT

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "clanAlt_Font.hpp"

enum HAlign : uint8_t {
	LEFT   = 0x1,
	RIGHT  = 0x2,
	CENTER = 0x3
};

enum VAlign : uint8_t {
	TOP    = 0x1,
	BOTTOM = 0x2,
	MIDDLE = 0x3
};

//! Align nibble. LS2B -> HAlign, MS2B ->VAlign
enum Alignment : uint8_t {
	_HALIGN_MASK   = 0x03, // `0b0011`
	_VALIGN_MASK   = 0x0C, // `0b1100`

	LEFT_TOP      = HAlign::LEFT + (VAlign::TOP << 2),
	LEFT_BOTTOM   = HAlign::LEFT + (VAlign::BOTTOM << 2),
	LEFT_MIDDLE   = HAlign::LEFT + (VAlign::MIDDLE << 2),
	RIGHT_TOP     = HAlign::RIGHT + (VAlign::TOP << 2),
	RIGHT_BOTTOM  = HAlign::RIGHT + (VAlign::BOTTOM << 2),
	RIGHT_MIDDLE  = HAlign::RIGHT + (VAlign::MIDDLE << 2),
	CENTER_TOP    = HAlign::CENTER + (VAlign::TOP << 2),
	CENTER_BOTTOM = HAlign::CENTER + (VAlign::BOTTOM << 2),
	CENTER_MIDDLE = HAlign::CENTER + (VAlign::MIDDLE << 2),

	//// Short-form aliases
	LT = LEFT_TOP,
	LB = LEFT_BOTTOM,
	LM = LEFT_MIDDLE,
	RT = RIGHT_TOP,
	RB = RIGHT_BOTTOM,
	RM = RIGHT_MIDDLE,
	CT = CENTER_TOP,
	CB = CENTER_BOTTOM,
	CM = CENTER_MIDDLE,

	LC = LEFT_MIDDLE,
	RC = RIGHT_MIDDLE,
	CC = CENTER_MIDDLE
};

inline uint8_t operator*(const HAlign &h)    { return static_cast<uint8_t>(h); }
inline uint8_t operator*(const VAlign &v)    { return static_cast<uint8_t>(v); }
inline uint8_t operator*(const Alignment &a) { return static_cast<uint8_t>(a); }

inline HAlign get_halign(Alignment const &a) {
	return static_cast<HAlign>( *Alignment::_HALIGN_MASK & *a      );
}

inline VAlign get_valign(Alignment const &a) {
	return static_cast<VAlign>((*Alignment::_VALIGN_MASK & *a) >> 2);
}

inline bool operator==(Alignment const &a, HAlign const &h) {
	return h == get_halign(a);
}

inline bool operator==(Alignment const &a, VAlign const &v) {
	return v == get_valign(a);
}

inline Alignment make_alignment(HAlign const &h, VAlign const &v)
{
	return static_cast<Alignment>( (*h & 0x3) + ((*v & 0x3) << 2) );
}


template< typename T >
inline clan::Rectx<T> align(const Alignment &align, const clan::Rectx<T>& area, const clan::Sizex<T>& size)
{
	HAlign h = get_halign(align);
	VAlign v = get_valign(align);

	clan::Rectx<T> value;

	switch(h) {
	case HAlign::LEFT:
		value.left   = area.left;
		value.right  = area.left + size.width;
		break;
	case HAlign::RIGHT:
		value.left   = area.right - size.width;
		value.right  = area.right;
		break;
	case HAlign::CENTER:
		value.left   = (area.left + area.right - size.width) / T(2);
		value.right  = value.left + size.width;
		break;
	}

	switch(v) {
	case VAlign::TOP:
		value.top    = area.top;
		value.bottom = area.top + size.height;
		break;
	case VAlign::BOTTOM:
		value.top    = area.bottom - size.height;
		value.bottom = area.bottom;
		break;
	case VAlign::MIDDLE:
		value.top    = (area.top + area.bottom - size.height) / T(2);
		value.bottom = value.top + size.height;
		break;
	}

	return value;
}

inline void draw_aligned_text(
		clan::Canvas       &canvas,
		SimpleFont         &font,
		clan::Rectf  const &area,
		Alignment    const &alignment,
		std::string  const &text,
		clan::Colorf const &color = clan::Colorf::white
) {
	clan::Size size = font.get_text_rect(text).get_size();
	clan::Pointf point;

	if (size.width > area.get_width()) {
		// Draw at half-width and cross-fingers.
		size.width /= 2;
		point    = align(alignment, area, clan::Sizef{ size }).get_bottom_left();
		point.y -= font.get_glyph_metric().get_descent();

		clan::Mat4f const oldTransform = canvas.get_transform();
		clan::Mat4f newTransform = oldTransform;
		newTransform
			.translate_self(point.x, point.y, 0.0f)
			.scale_self(0.5f, 1.0f, 0.0f);
		canvas.set_transform(newTransform);
		font.draw_text(canvas, { 0, 0 }, text, color);
		canvas.set_transform(oldTransform);
	} else {
		point    = align(alignment, area, clan::Sizef{ size }).get_bottom_left();
		point.y -= font.get_glyph_metric().get_descent();
		font.draw_text(canvas, clan::Point{ point }, text, color);
	}
}

inline void draw_aligned_text(
		clan::Canvas       &canvas,
		clan::Font         &font,
		clan::Rectf  const &area,
		Alignment    const &alignment,
		std::string  const &text,
		clan::Colorf const &color = clan::Colorf::white
) {
	clan::Sizef size = font.measure_text(canvas, text).bbox_size;
	clan::Pointf point;

	if (size.width > area.get_width()) {
		// Draw at half-width and cross-fingers.
		size.width /= 2.0f;
		point    = align(alignment, area, size).get_bottom_left();
		point.y -= font.get_font_metrics().get_descent();

		clan::Mat4f const oldTransform = canvas.get_transform();
		clan::Mat4f newTransform = oldTransform;
		newTransform
			.translate_self(point.x, point.y, 0.0f)
			.scale_self(0.5f, 1.0f, 0.0f);
		canvas.set_transform(newTransform);
		font.draw_text(canvas, { 0, 0 }, text, color);
		canvas.set_transform(oldTransform);
	} else {
		point    = align(alignment, area, size).get_bottom_left();
		point.y -= font.get_font_metrics().get_descent();
		font.draw_text(canvas, point, text, color);
	}
}

#endif
