//  UI/Slider.hpp :: Custom slider
//  Copyright 2014 Keigen Shu

#ifndef UI_SLIDER_H
#define UI_SLIDER_H

#include "../__zzCore.hpp"
#include "Common.hpp"

namespace UI
{

/** A simple slider component.
 *  Values are in the range of [mMin : mMax] (mMin inclusive to mMax inclusive).
 */
class Slider : public clan::View
{
public:
	using ThumbState = Common::Switch;

	enum Direction {
		RIGHT = 0x0,
		LEFT  = 0x1,
		DOWN  = 0x2,
		UP    = 0x3
	};

private:
	////    Operational parameters    ////
	Direction mDirection;

	int mDefault; //!< The default value of the slider.
	int mMin;     //!< The lower limit of the range of the slider
	int mMax;     //!< The upper limit of the range of the slider
	int mValue;   //!< Current value of the slider.

public:
	////    GUI Customization    ////
	clan::Colorf    mcBackground;   //!< Slider background color.
	clan::Colorf    mcRangeGuide;   //!< Slider range guide decal color.
	clan::Colorf    mcThumbRail;    //!< Slider thumb rail/slot color.
	clan::Colorf    mcThumb    [4]; //!< 2 bits :: MSB = hovered?, LSB = moving?, [0b01 is never used]
	clan::Colorf    mcThumbLine[2]; //!< Slider thumb guide line color [Not held, Holding]

protected:
	////    Slider Thumb state variables    ////
	ThumbState  mThumbState;        //!< Switch == isHeldDown; Changing == isHovered
	point2f     mPointerPosition;   //!< Location of the mouse pointer on the previous update.

private:
	////    Class callbacks    ////
	clan::Signal<void()> mfValueChanged;

protected:
	////    Convenience functions    ////
	void get_thumb_area(float &p, rectf &r) const;

public:
	Slider();

	////    Class Attributes    ////
	inline int get_default() const { return mDefault; }
	inline int get_min() const { return mMin; }
	inline int get_max() const { return mMax; }
	inline int get_value() const { return mValue; }

	inline Direction const &get_direction() const { return mDirection; }
	inline void set_direction(Direction const &d) { mDirection = d; }

	/** Sets the value of the slider.
	 *  \note Requests a component repaint.
	 *  \note Invokes `ValueChanged` callback.
	 */
	void set_value(int v);
	void set_range(int a, int b, int d);

	////    clan::View overrides    ////
	void render_content(clan::Canvas &canvas) override;

	////    Class interaction callback    ////
	inline clan::Signal<void()>& func_value_changed() { return mfValueChanged; }

private:
	////    Class UI callback listeners    ////
	void on_pointer_leave(clan::PointerEvent &event);
	void on_pointer_move(clan::PointerEvent &event);
	void on_pointer_press(clan::PointerEvent &event);
	void on_pointer_release(clan::PointerEvent &event);

};


}

#endif

