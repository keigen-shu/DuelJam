//  UI/Tooltip.hpp :: A tooltip pop-up
//  Copyright 2014 Keigen Shu

#ifndef UI_TOOLTIP_H
#define UI_TOOLTIP_H

#include "Common.hpp"

namespace UI
{

class Tooltip : public clan::GUIComponent
{
private:
    clan::GUIComponent* mParent;
    clan::Colorf        mFgColor;
    clan::Colorf        mBgColor;
    clan::Colorf        mBorderColor;


public:

};


}

#endif

