#include "SwitchButton.hpp"
#include <cassert>

namespace UI {

SwitchButton::SwitchButton()
	: clan::View()
	, mButtonState      (Common::Switch::OFF)
	, mProximityState   (Common::Switch::OFF)
{
	////    Default colors    ////
	mcBorder[0] = {0.4f, 0.4f, 0.4f}; // not pressed
	mcBorder[1] = {0.3f, 0.3f, 0.3f}; // PRESSED
	mcBorder[2] = {0.6f, 0.6f, 0.8f}; // HOVER + not pressed
	mcBorder[3] = {0.5f, 0.5f, 0.7f}; // HOVER + PRESSED
	mcBorder[4] = {0.2f, 0.2f, 0.2f}; // PRESSING

	mcBackground[0] = {0.8f, 0.8f, 0.8f};
	mcBackground[1] = {0.6f, 1.0f, 0.6f};
	mcBackground[2] = {0.8f, 0.8f, 1.0f};
	mcBackground[3] = {0.8f, 1.0f, 1.0f};
	mcBackground[4] = {0.4f, 1.0f, 0.6f};

	////    Component setup    ////
	set_focus_policy(clan::FocusPolicy::accept);

	slots.connect(sig_pointer_enter(), this, &SwitchButton::on_pointer_enter);
	slots.connect(sig_pointer_leave(), this, &SwitchButton::on_pointer_leave);
	slots.connect(sig_pointer_press(), this, &SwitchButton::on_pointer_press);
	slots.connect(sig_pointer_release(), this, &SwitchButton::on_pointer_release);
}

void SwitchButton::render_content(clan::Canvas &canvas)
{
	uchar i = Common::isChanging(mButtonState) ?
		4 : ( 0 +
		((mProximityState & Common::Switch::__STATE) << 1) +
		((mButtonState    & Common::Switch::__STATE) << 0) );

	canvas.fill_rect(1, 1, box_style.width()-1, box_style.height()  , mcBackground[i]);

	canvas.draw_line(1, 1, box_style.width()-1, 1, mcBorder[i]); // Top
	canvas.draw_line(1, 1, 1, box_style.height()-1, mcBorder[i]); // Left
	canvas.draw_line(box_style.width()-1, 1, box_style.width(), box_style.height()-1, mcBorder[i]); // Right
	canvas.draw_line(1, box_style.height()-1, box_style.width()-1, box_style.height(), mcBorder[i]); // Bottom
}


void SwitchButton::on_pointer_press(clan::PointerEvent &event)
{
	if (event.button() != clan::PointerButton::left)
		return;

	Common::setChanging(mButtonState);
	set_needs_render();
	event.stop_propagation();
}

void SwitchButton::on_pointer_release(clan::PointerEvent &event)
{
	if (event.button() != clan::PointerButton::left)
		return;

	Common::resetChanging(mButtonState);
	mButtonState = ~mButtonState;

	mfButtonStateChanged(Common::isOn(mButtonState));

	set_needs_render();
	event.stop_propagation();
}

void SwitchButton::on_pointer_enter(clan::PointerEvent &event)
{
	mProximityState = +mProximityState;
	set_needs_render();
	event.stop_propagation();
}

void SwitchButton::on_pointer_leave(clan::PointerEvent &event)
{
	mProximityState = -mProximityState;
	set_needs_render();
	event.stop_propagation();
}

}
