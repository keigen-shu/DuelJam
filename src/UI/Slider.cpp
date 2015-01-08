#include <cassert>
#include "Slider.hpp"
#include "Common.hpp"

namespace UI {

constexpr int   kPadding = 4;

////    Internal functions    ////
// a <= x <= b
inline bool is_in_range(int const &value, int const &a, int const &b) {
	return (b < a) ? is_in_range(value, b, a) : (value >= a && value <= b);
}

inline float get_normalized_value(int const &value, int const &a, int const &b) {
	return (b < a) ? get_normalized_value(value, b, a) : (static_cast<float>(value - a) / static_cast<float>(b - a));
}

////    Class attributes    ////
void Slider::get_thumb_area(float &p, rectf &r) const
{
	p = get_normalized_value(mValue, mMin, mMax);

	switch (mDirection) {
	case Slider::Direction::RIGHT: p =      p  * (box_style.width() - 2 * kPadding - 1); break;
	case Slider::Direction::LEFT : p = (1.f-p) * (box_style.width() - 2 * kPadding - 1); break;
	case Slider::Direction::DOWN : p =      p  * (box_style.height() - 2 * kPadding - 1); break;
	case Slider::Direction::UP   : p = (1.f-p) * (box_style.height() - 2 * kPadding - 1); break;
	}

	p = std::round(p) + kPadding;

	/****/ if (mDirection >= Slider::Direction::DOWN) { // Vertical slider
		r = rectf{ 0.f, p - kPadding, box_style.width(), p + kPadding + 1.f };
	} else { // Horizontal slider
		r = rectf{ p - kPadding, 0.f, p + kPadding + 1.f, box_style.height() };
	}
	p = p + 1.f;
}

void Slider::set_value(int val)
{
	mValue = val;

	if (is_in_range(val, mMin, mMax) == false)
		clan::Console::write_line(
				"UI::Slider::set_position() [warn] "
				"Supplied position '%1' is not within range ['%2' to '%3'].",
				mDefault, mMin, mMax
				);

	//  TODO Tooltip?
	//  Repaint component.
	set_needs_render();

	//  Invoke user callback.
	mfValueChanged();
}

void Slider::set_range(int a, int b, int d) {
	mMin = std::min(a, b);
	mMax = std::max(a, b);
	mDefault = d;

	if (is_in_range(d, mMin, mMax) == false)
		clan::Console::write_line(
				"UI::Slider::set_ranges() [warn] "
				"Supplied default value '%1' is not within range ['%2' to '%3'].",
				mDefault, mMin, mMax
				);
}


////    Constructor    ////

Slider::Slider()
	: clan::View()
	, mDirection(Slider::Direction::RIGHT)
	, mMin  (-1)
	, mMax  ( 1)
	, mValue( 0)
	, mThumbState()
	, mPointerPosition()
{
	////    Default colors    ////
	mcBackground   = clan::Colorf{ 0.95f, 0.95f, 0.95f };
	mcRangeGuide   = clan::Colorf{ 0.75f, 0.75f, 0.75f };
	mcThumbRail    = clan::Colorf{ 0.6f, 0.6f, 0.6f };
	mcThumb    [0] = clan::Colorf{ 0.0f, 0.0f, 0.0f };
	mcThumb    [1] = clan::Colorf{ 0.0f, 0.8f, 0.0f };
	mcThumb    [2] = clan::Colorf{ 0.0f, 0.0f, 0.4f };
	mcThumb    [3] = clan::Colorf{ 0.0f, 0.8f, 0.4f };
	mcThumbLine[0] = clan::Colorf{ 0.8f, 0.8f, 0.8f };
	mcThumbLine[1] = clan::Colorf{ 1.0f, 1.0f, 1.0f };

	////    Component setup    ////
	set_focus_policy(clan::FocusPolicy::accept);

	slots.connect(sig_pointer_leave(), this, &Slider::on_pointer_leave);
	slots.connect(sig_pointer_move(), this, &Slider::on_pointer_move);
	slots.connect(sig_pointer_press(), this, &Slider::on_pointer_press);
	slots.connect(sig_pointer_release(), this, &Slider::on_pointer_release);
}


////    Class GUI callback listeners    ////

void Slider::render_content(clan::Canvas &canvas)
{
	//  Background
	canvas.fill_rect(0, 0, box_style.width(), box_style.height(), mcBackground);

	//  Slider Thumb
	float p = 0.f;
	rectf r { 0.f, 0.f, 0.f, 0.f };
	get_thumb_area(p, r);

	const point2f midpoint { box_style.width() / 2, box_style.height() / 2 };
	/****/ if (mDirection >= Slider::Direction::DOWN) { // Vertical slider
		//  Railing and marker
		canvas.draw_line(midpoint.x, kPadding, midpoint.x, box_style.height() - kPadding + 1, mcThumbRail);

		canvas.draw_line(midpoint.x           ,                  1, box_style.width()    ,                  1, mcRangeGuide); // -
		canvas.draw_line(box_style.width() - 2,                  1, box_style.width() - 3, box_style.height(), mcRangeGuide); // |
		canvas.draw_line(midpoint.x           , box_style.height(), box_style.width()    , box_style.height(), mcRangeGuide); // -

		canvas.fill_rect(r, mcThumb[*mThumbState]);
		canvas.draw_line(0, p, box_style.width(), p, mcThumbLine[Common::isOn(mThumbState)]);
	} else { // Horizontal slider
		//  Railing and marker
		canvas.draw_line(kPadding, midpoint.y, box_style.width() - kPadding + 1, midpoint.y, mcThumbRail);

		canvas.draw_line(                1, midpoint.y            ,                 1, box_style.height()    , mcRangeGuide); // |
		canvas.draw_line(                1, box_style.height() - 2, box_style.width(), box_style.height() - 3, mcRangeGuide); // -
		canvas.draw_line(box_style.width(), midpoint.y            , box_style.width(), box_style.height()    , mcRangeGuide); // |

		canvas.fill_rect(r, mcThumb[*mThumbState]);
		canvas.draw_line(p, 0, p, box_style.height(), mcThumbLine[Common::isOn(mThumbState)]);
	}
}

void Slider::on_pointer_leave(clan::PointerEvent &event)
{
	Common::resetChanging(mThumbState);
	event.stop_propagation();
}

////    Class GUI callback listener functions    ////

void Slider::on_pointer_move(clan::PointerEvent &event)
{
	float p = 0.f; // Unused float from get_thumb_area.
	rectf r { 0.f, 0.f, 0.f, 0.f };
	get_thumb_area(p, r);

	if (r.contains(event.pos())) {
		Common::setChanging(mThumbState);
	} else {
		Common::resetChanging(mThumbState);
	}

	if (Common::isOn(mThumbState))
	{
		point2f new_pos = to_screen_pos(event.pos());
		// clan::Console::write_line("(%1,%2)", new_pos.x, new_pos.y);

		char incr = 0;
		switch (mDirection) {
			case Direction::RIGHT:
				incr = new_pos.x - mPointerPosition.x;
				break;
			case Direction::LEFT:
				incr = mPointerPosition.x - new_pos.x;
				break;
			case Direction::DOWN:
				incr = new_pos.y - mPointerPosition.y;
				break;
			case Direction::UP:
				incr = mPointerPosition.y - new_pos.y;
				break;
		}

		incr = incr / std::abs(incr); // Strip magnitude

		if (incr == 0) {
			return;
		} else {
			// Take new value if it is inside the slider range.
			set_value(is_in_range(mValue + incr, mMin, mMax)
					? mValue + incr
					// or take the limit that is closer to the value.
					: mValue
					);
		}

		mPointerPosition = new_pos;

		event.stop_propagation();
		return;
	}

	// If pointer on top of thumb, make it pretty or something
}

void Slider::on_pointer_press(clan::PointerEvent &event)
{
	if (event.button() == clan::PointerButton::left)
	{
		if (Common::isOff(mThumbState))
		{
			mThumbState = +mThumbState;
			mPointerPosition = event.pos();
			// capture_mouse(true);
			set_needs_render();
		}
	}
	else
	{
		return;
	}

	event.stop_propagation();
}

void Slider::on_pointer_release(clan::PointerEvent &event)
{
	if (event.button() == clan::PointerButton::left)
	{
		if (Common::isOn(mThumbState))
		{
			mThumbState = -mThumbState;
			mPointerPosition = event.pos();
			// capture_mouse(false);
			set_needs_render();
		}
	}
	else if (event.button() == clan::PointerButton::middle)
	{
		// Middle-click -> reset to default.
		set_value(mDefault);
	}
	else
	{
		return;
	}

	event.stop_propagation();
}

}
