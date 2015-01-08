/* __zzCore.hpp :: Core application definitions
 * Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>
 */

#ifndef DUELJAM_CORE_DEFINES
#define DUELJAM_CORE_DEFINES

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/ui.h>

typedef clan::Quad      quadi;
typedef clan::Rect      recti;
typedef clan::Size      sizei;
typedef clan::Vec2i     vec2i;
typedef clan::Line2     line2i;
typedef clan::Point     point2i;
typedef clan::Color     color;

typedef clan::Quadf     quadf;
typedef clan::Rectf     rectf;
typedef clan::Sizef     sizef;
typedef clan::Vec2f     vec2f;
typedef clan::Line2f    line2f;
typedef clan::Pointf    point2f;
typedef clan::Colorf    colorf;

typedef clan::Quadd     quadd;
typedef clan::Rectd     rectd;
typedef clan::Sized     sized;
typedef clan::Vec2d     vec2d;
typedef clan::Line2d    line2d;
typedef clan::Pointd    point2d;

/*  -- ClanLib HELPERS --  */
////////////////////////////////////////////////////////////

inline void dump_event(clan::InputEvent const& event, std::string const& from = "core")
{
    clan::InputEvent::Type const& et = event.type;
    std::string type =
        et == clan::InputEvent::Type::no_key            ? "No Key"      :
        et == clan::InputEvent::Type::pressed           ? "Pressed"     :
        et == clan::InputEvent::Type::released          ? "Released"    :
        et == clan::InputEvent::Type::doubleclick       ? "dblClick"    :
        et == clan::InputEvent::Type::pointer_moved     ? "pntrMoved"   :
        et == clan::InputEvent::Type::axis_moved        ? "axisMoved"   :
        et == clan::InputEvent::Type::proximity_change  ? "proxChange"  : "invalid";

    clan::InputDevice::Type const& dt = event.device.get_type();
    std::string device =
        dt == clan::InputDevice::Type::joystick ? "joystick"    :
        dt == clan::InputDevice::Type::keyboard ? "keyboard"    :
        dt == clan::InputDevice::Type::pointer  ? "pointer"     :
        dt == clan::InputDevice::Type::tablet   ? "tablet"      :
        dt == clan::InputDevice::Type::unknown  ? "unknown"     : "invalid";

    std::string mod =
        std::string(event.alt   ? "[Alt] " : "") +
        std::string(event.ctrl  ? "[Ctl] " : "") +
        std::string(event.shift ? "[Sft] " : "");

    clan::Console::write_line("%1 [dump] got input event (repeat_count = %2)", from, event.repeat_count);
    clan::Console::write_line("    type %1 from %2", type, device);
    clan::Console::write_line("    ID = %1 + %2 + '%3' --> '%4'", event.id, event.id_offset, mod, event.str);
    clan::Console::write_line("    mouse_pos = (%1,%2), axis_pos = %3", event.mouse_pos.x, event.mouse_pos.y, event.axis_pos);
}

template <typename T>
inline clan::Rectx<T> product(const clan::Rectx<T>& area, const clan::Sizex<T>& size)
{
    T l = area.left * size.width;
    T r = area.right * size.width;
    T u = area.top * size.height;
    T d = area.bottom * size.height;

    return clan::Rectx<T> (l, u, r, d);
}

////    CONFIGURATION DEFINITIONS    ///////////////////////

////    Use Direct3D instead of the default OpenGL on Windows?
#define USE_D3D_ON_WINDOWS

#if defined(USE_D3D_ON_WINDOWS) && ( defined(_WIN32) || defined(_WIN64) )
#define __USE_D3D
#endif

#endif
