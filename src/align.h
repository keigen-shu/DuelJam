/*
**  ClanLib SDK
**  Copyright (c) 1997-2014 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty. In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  Fiile Author(s):
**
**    Chu Chin Kuan
**
*/

#pragma once

#include <cstdint>

namespace clan
{

//! Horizontal alignment enumerator.
//! \addtogroup clanCore_Math
enum class HAlign : uint8_t {
	left   = 0x1,
	right  = 0x2,
	center = 0x3,
	middle = 0x3
};


//! Vertical alignment enumerator.
//! \addtogroup clanCore_Math
enum class VAlign : uint8_t {
	top    = 0x1,
	bottom = 0x2,
	middle = 0x3,
	center = 0x3
};


/*! 2-dimensional alignment nibble enumerator.
 *  \addtogroup clanCore_Math
 *
 *  The least significant 2 bits denote the horizontal alignment (HAlign) while
 *  the next 2 bits denote the vertical alignment (VAlign). The remaining 4 bits
 *  on the byte can be used for something else, depending on the need.
 */
enum class Alignment : uint8_t {
	//! \name Bit masks
	//! \{
	_halign_mask    = 0x3, //!< Horizontal alignment part mask `0b0011`
	_valign_mask    = 0xC, //!< Vertical alignment part mask `0b1100`
	//! \}

	// Formula: HAlign | (VAlign << 2)
	left_top        = 0x1 | (0x1 << 2),
	left_bottom     = 0x1 | (0x2 << 2),
	left_middle     = 0x1 | (0x3 << 2),

	right_top       = 0x2 | (0x1 << 2),
	right_bottom    = 0x2 | (0x2 << 2),
	right_middle    = 0x2 | (0x3 << 2),

	center_top      = 0x3 | (0x1 << 2),
	center_bottom   = 0x3 | (0x2 << 2),
	center_middle   = 0x3 | (0x3 << 2),

	//! \name Handy short-form aliases
	//! \{
	lt = left_top,
	lb = left_bottom,
	lm = left_middle,
	rt = right_top,
	rb = right_bottom,
	rm = right_middle,
	ct = center_top,
	cb = center_bottom,
	cm = center_middle,

	tl = left_top,
	tr = right_top,
	tc = center_top,
	bl = left_bottom,
	br = right_bottom,
	bc = center_bottom,
	ml = left_middle,
	mr = right_middle,
	mc = center_middle,

	lc = left_middle,
	rc = right_middle,

	cl = left_middle,
	cr = right_middle,

	cc = center_middle,
	mm = center_middle
	//! \}
};

//! \name Alignment attribute getters and operator overloads.
//! \addtogroup clanCore_Math
//! \{

//! Cast-to-byte operator overload for HAlign.
inline uint8_t operator*(const HAlign &h)    { return static_cast<uint8_t>(h); }

//! Cast-to-byte operator overload for VAlign.
inline uint8_t operator*(const VAlign &v)    { return static_cast<uint8_t>(v); }

//! Cast-to-byte operator overload for Alignment.
inline uint8_t operator*(const Alignment &a) { return static_cast<uint8_t>(a); }


//! Retrieves the horizontal alignment for the Alignment byte.
inline HAlign get_halign(Alignment const &a) { return static_cast<HAlign>(*a      & *Alignment::_halign_mask); }

//! Retrieves the vertical alignment for the Alignment byte.
inline VAlign get_valign(Alignment const &a) { return static_cast<VAlign>(*a >> 2 & *Alignment::_valign_mask); }


/*! Compares only the horizontal part of the alignment.
 *  \code
 *  bool t = Alignment::LC == HAlign::left; // true
 *  bool f = Alignment::CC == HAlign::left; // false
 *  \endcode
 */
inline bool operator==(Alignment const &a, HAlign const &h) { return h == get_halign(a); }
inline bool operator!=(Alignment const &a, HAlign const &h) { return h != get_halign(a); }
inline bool operator==(HAlign const &h, Alignment const &a) { return h == get_halign(a); }
inline bool operator!=(HAlign const &h, Alignment const &a) { return h != get_halign(a); }

/*! Compares only the vertical part of the alignment.
 *  \code
 *  bool t = Alignment::LT == HAlign::TOP; // true
 *  bool f = Alignment::LB == HAlign::BOTTOM; // false
 *  \endcode
 */
inline bool operator==(Alignment const &a, VAlign const &v) { return v == get_valign(a); }
inline bool operator!=(Alignment const &a, VAlign const &v) { return v != get_valign(a); }
inline bool operator==(VAlign const &v, Alignment const &a) { return v == get_valign(a); }
inline bool operator!=(VAlign const &v, Alignment const &a) { return v != get_valign(a); }


//! Creates an alignment object.
inline Alignment make_alignment(HAlign const &h, VAlign const &v) { return static_cast<Alignment>( (*h & *Alignment::_halign_mask) | (*v << 2 & *Alignment::_valign_mask) ); }
//! Creates an alignment object.
inline Alignment make_alignment(VAlign const &v, HAlign const &h) { return static_cast<Alignment>( (*h & *Alignment::_halign_mask) | (*v << 2 & *Alignment::_valign_mask) ); }


//! \name Alignment-based methods
//! \{

//! Retrieves the barycentric coordinate of the Alignment byte.
//! This function is intended to be used with the `mix` function.
inline Point2f get_point(Alignment const &align)
{
	const HAlign h = get_halign(align);
	const VAlign v = get_valign(align);

	clan::Point2f value;

	switch(h) {
	case HAlign::left  : value.x = 0.0f; break;
	case HAlign::right : value.x = 1.0f; break;
	case HAlign::center: value.x = 0.5f; break;
	}

	switch(v) {
	case VAlign::top   : value.y = 0.0f; break;
	case VAlign::bottom: value.y = 1.0f; break;
	case VAlign::middle: value.y = 0.5f; break;
	}

	return value;
}


/*! Creates a Rect of supplied size that is anchored to the supplied
 *  alignment inside the supplied area.
 *
 *  \code
 *  float W = 0.7f;
 *  float H = 1.1f;
 *  clan::Rectf AREA (1.0f, 2.0f, 3.0f, 5.0f);
 *
 *  clan::Rectf R = align(Alignment::RIGHT_TOP, AREA, clan::Sizef(W, H));
 *
 *  //       W
 *  // +----+-+
 *  // |    |R| H
 *  // |    +-x
 *  // | AREA |
 *  // +------x
 *  //
 *  // R.left   = 3.0f - W: 0.7f = 2.3f;
 *  // R.top    = 2.0f;
 *  // R.right  = 3.0f;
 *  // R.bottom = 2.0f + H: 1.1f = 3.3f;
 *  \endcode
 *
 *  \param align Anchor position of the new Rect inside `area`.
 *  \param area  Area to fit the new Rect into.
 *  \param size  Size of the new Rect.
 */
template< typename T >
Rectx<T> align(const Alignment &align, const Rectx<T>& area, const Sizex<T>& size)
{
	HAlign h = get_halign(align);
	VAlign v = get_valign(align);

	clan::Rectx<T> value;

	switch(h) {
	case HAlign::left:
		value.left   = area.left;
		value.right  = area.left + size.width;
		break;
	case HAlign::right:
		value.left   = area.right - size.width;
		value.right  = area.right;
		break;
	case HAlign::center:
		value.left   = (area.left + area.right - size.width) / T(2);
		value.right  = value.left + size.width;
		break;
	}

	switch(v) {
	case VAlign::top:
		value.top    = area.top;
		value.bottom = area.top + size.height;
		break;
	case VAlign::bottom:
		value.top    = area.bottom - size.height;
		value.bottom = area.bottom;
		break;
	case VAlign::middle:
		value.top    = (area.top + area.bottom - size.height) / T(2);
		value.bottom = value.top + size.height;
		break;
	}

	return value;
}

/*! Creates a Rect of the supplied size so that the supplied point is anchored
 *  onto Rect at the supplied alignment position.
 *
 *  This function works like Rectx<T>::Rectx(Pointx<T>, Sizex<T>), but
 *  with the additional option to select the direction to "expand" to.
 *
 *  \code
 *  float W = 0.7f;
 *  float H = 1.1f;
 *  clan::Point2f POINT (1.0f, 2.0f);
 *
 *  clan::Rectf R = align(Alignment::RIGHT_TOP, POINT, clan::Sizef(W, H));
 *
 *  //   W
 *  // +---x <- POINT
 *  // | R | H
 *  // +---+
 *  //
 *  // R.left   = 1.0f - W: 0.7f = 0.3f;
 *  // R.top    = 2.0f;
 *  // R.right  = 1.0f;
 *  // R.bottom = 2.0f + H: 1.1f = 3.3f;
 *  \endcode
 *
 *  \param align Anchor position of the new Rect inside `area`.
 *  \param point Point to anchor the new Rect on.
 *  \param size  Size of the new Rect.
 */
template< typename T >
Rectx<T> align(const Alignment &align, const Pointx<T>& point, const Sizex<T>& size)
{
	HAlign h = get_halign(align);
	VAlign v = get_valign(align);

	clan::Rectx<T> value;

	switch(h) {
	case HAlign::left:
		value.left   = point.x;
		value.right  = point.x + size.width;
		break;
	case HAlign::right:
		value.left   = point.x - size.width;
		value.right  = point.x;
		break;
	case HAlign::center:
		value.left   = point.x - (size.width / T(2));
		value.right  = value.left + size.width;
		break;
	}

	switch(v) {
	case VAlign::top:
		value.top    = point.y;
		value.bottom = point.y + size.height;
		break;
	case VAlign::bottom:
		value.top    = point.y - size.height;
		value.bottom = point.y;
		break;
	case VAlign::middle:
		value.top    = point.y - (size.height / T(2));
		value.bottom = value.top + size.height;
		break;
	}

	return value;
}

//! \}
//! \}

} // namespace clan
