//  Loop.hpp :: Looping sequence object
//  Copyright 2012 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_LOOP_H
#define AWE_LOOP_H

#include "Define.hpp"

#include <algorithm>
#include <bitset>
#include <sstream>

namespace awe {
namespace Loop {

/*! Loop operation mode enumerator.
 *
 *  The first two least significant bits is the loop traversal method.
 *  The third bit is not used but reserved for future additions.
 *  The forth bit is the current loop movement direction.
 */
enum class Mode : uint8_t {
    UNDEFINED   = 0x0, //!< Unused and treated the same as `ONCE`.

    ONCE        = 0x1, //!< Traverse slice from beginning to end only once.
    REPEAT      = 0x2, //!< Traverse slice from beginning to end and repeat forever.
    ALTERNATING = 0x3, //!< Traverse slice from beginning to end then back and repeat forever.

    FORWARD     = 0x0, //!< Traverse in normal direction (begin to end).
    REVERSE     = 0x8, //!< Traverse in reverse direction (end to begin).

    /** Default loop mode: `[ FORWARD | ONCE ]` */
    __DEFAULT   = FORWARD | ONCE,

    __LOOP_MODE = 0x3, //!< Loop mode bit mask.
    __DIRECTION = 0x8, //!< Loop direction bit mask.
};

//!@name Global Methods and Operator Overrides for Mode
//!@{

inline static uint8_t operator* (const Mode&  mode) { return static_cast<uint8_t>(mode); }
inline static uint8_t operator* (const Mode&& mode) { return static_cast<uint8_t>(mode); }

inline static bool    isReverse (const Mode &mode) { return (*mode & *Mode::__DIRECTION) == *Mode::__DIRECTION; }
inline static bool    isForward (const Mode &mode) { return !isReverse(mode); }

/*! Retrieves the loop traversal method segment of the Mode enumerator.
 *  @see Mode Enumerator definition.
 */
inline static Mode getMethod (const Mode &mode) { return static_cast<Mode>(*mode & *Mode::__LOOP_MODE); } /* b'00000111' */

inline static Mode operator& (const Mode &a, const Mode &b) { return static_cast<Mode>(*a & *b); } //!< Bit-wise AND operator
inline static Mode operator| (const Mode &a, const Mode &b) { return static_cast<Mode>(*a | *b); } //!< Bit-wise  OR operator
inline static Mode operator^ (const Mode &a, const Mode &b) { return static_cast<Mode>(*a ^ *b); } //!< Bit-wise XOR operator

//! Inverts the loop direction
inline static Mode operator~ (const Mode &mode) { return static_cast<Mode>(*mode ^  *Mode::__DIRECTION); }

//! Forces the direction to forward
inline static Mode operator+ (const Mode &mode) { return static_cast<Mode>(*mode & ~*Mode::__DIRECTION); /* b'11110111' */ }

//! Forces the direction to reverse
inline static Mode operator- (const Mode &mode) { return static_cast<Mode>(*mode |  *Mode::__DIRECTION); /* b'00001000' */ }

//!@}

//! Class managing loop sequence progression.
template< typename T >
class Loop
{
public:
    Mode    mode;   //!< Looping mode.
    bool    paused; //!< Stops movement if set to true.
    T       begin;  //!< The beginning offset of the loop.
    T       now;    //!< The current offset of the loop.
    T       end;    //!< The ending offset of the loop.

    Loop(T _begin, T _end,         Mode _mode = Mode::__DEFAULT, bool _paused = false)
        : mode(_mode)
        , paused(_paused)
        , begin(_begin)
        , now(isReverse(_mode) ? _end : _begin)
        , end(_end)
    { }

    Loop(T _begin, T _end, T _now, Mode _mode = Mode::__DEFAULT, bool _paused = false)
        : mode(_mode)
        , paused(_paused)
        , begin(_begin)
        , now(_now)
        , end(_end)
    { }

    /*! Moves the current position by `b`.
     *  @return true if the loop is/has been paused.
     */
    bool operator+=(T b) {
        if (paused) {
            return true;
        }

        switch (getMethod(mode)) {
        case Mode::UNDEFINED:
#ifdef DEBUG
            printf("libawe [warn] Undefined loop mode on object %p.\n", this);
#endif
        case Mode::ONCE:
            if (isForward(mode)) {
                now += b;
                if (now > end) {
                    now = end;
                    paused = true;
                }
            } else {
                now -= b;
                if (now < begin) {
                    now = begin;
                    paused = true;
                }
            }
            break;

        case Mode::REPEAT:
            if (isForward(mode)) {
                now += b;
            } else {
                now -= b;
            }

            if (now > end) {
                now = begin;
            } else if (now < begin) {
                now = end;
            }

            break;

        case Mode::ALTERNATING:
            if (isForward(mode)) {
                now += b;
            } else {
                now -= b;
            }

            if (now < begin || now > end) {
                mode = ~mode;
            }

            break;

        default:
            std::ostringstream error_string;
            error_string
                    << "Invalid loop mode {in binary " << std::bitset<8>(*mode)
                    << "} encountered on loop object " << std::hex << this
                    << "." << std::endl;
            throw std::invalid_argument(error_string.str().c_str());
        }

        return false;
    }

    /*! Moves the current position by `-b`.
     *  @return true if the loop is/has been paused.
     */
    inline bool operator-=(T b) {
        return operator+=(-b);
    }

    /*! Returns the starting position of the loop sequence as an unsigned long.
     *  @return Position of the beginning of the loop sequence.
     */
    inline unsigned long ubegin() const {
        return std::floor(begin);
    }

    /*! Returns the ending position of the loop sequence as an unsigned long.
     *  @return Position of the end of the loop sequence.
     */
    inline unsigned long uend() const {
        return std::floor(end);
    }

    /*! Returns the current position of the loop sequence as an unsigned long.
     *  @return Current position of the loop sequence.
     */
    inline unsigned long unow() const {
        return std::min<unsigned long>(now, uend() - 1);
    }

    /*! Returns the current position relative to the end of the loop sequence.
     *  @return Current position relative to end of loop.
     */
    inline double getPosition() const {
        return static_cast<double>(now) / static_cast<double>(end);
    }

    /*! Returns the current position relative to the range of the loop.
     *  @return Current position relative to traversal range of loop.
     */
    inline double getProgress() const {
        return static_cast<double>(now - begin) / static_cast<double>(end - begin);
    }
};

}

using AiLoop = Loop::Loop<   signed long >;
using AuLoop = Loop::Loop< unsigned long >;
using AfLoop = Loop::Loop<  float >;
using AdLoop = Loop::Loop< double >;

}

#endif

