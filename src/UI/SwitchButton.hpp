//  UI/SwitchButton.hpp :: Switch button UI Component
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "Common.hpp"

namespace UI {

class SwitchButton : public clan::View {
public:
    using ButtonState       = Common::Switch;
    using ProximityState    = Common::Switch;

protected:
    ButtonState     mButtonState;
    ProximityState  mProximityState;

public:
    clan::Colorf    mcBorder    [5]; // 2 bits :: MSB = hovered?, LSB = pushed?, or 4 (0b100) if pressing!
    clan::Colorf    mcBackground[5]; // So, 0 is unhovered and unpushed, 2 is hovered and unpushed, 3 is hovered and pushed

private:
    ////    Class callbacks    ////
    clan::Signal<void(bool)> mfButtonStateChanged;
    clan::Signal<void(bool)> mfProximityStateChanged;

public:
    SwitchButton();

    inline bool isOn() const { return Common::isOn(mButtonState); }
    clan::Signal<void(bool)> &func_toggled() { return mfButtonStateChanged; }

    void render_content(clan::Canvas &canvas) override;

private:
    void on_pointer_press(clan::PointerEvent &event);
    void on_pointer_release(clan::PointerEvent &event);

    void on_pointer_enter(clan::PointerEvent &event);
    void on_pointer_leave(clan::PointerEvent &event);
};

}
#endif
