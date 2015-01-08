//  clanExt_JSONReader.hpp :: JSON reader
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

#ifndef H_CLAN_EXT_JSON
#define H_CLAN_EXT_JSON

#include <ClanLib/core.h>
#include <ClanLib/display.h>


class JSONReader
{
protected:
    clan::JsonValue   mRoot;

public:
    static clan::JsonValue        getJsonValue(std::string const &path, clan::JsonValue       & root);
    static clan::JsonValue const cgetJsonValue(std::string const &path, clan::JsonValue const & root);

public:
    JSONReader();
    JSONReader(clan::JsonValue const &root);

    inline clan::JsonValue& getRoot() { return mRoot; }

    /** Loads a color object from the JSON file.
     *
     *  Color objects can be written in four (or eleven) forms:
     *
     *   1. a text string that matches any of ClanLib's predefined
     *      colors OR a hex value string of the color,
     *
     *          { "full-white": "white" }
     *          { "full-white": "#FFFFFF" }
     *          { "half-white": "#FFFFFF7F" }
     *
     *   2. a JSON array containing an element matching the above,
     *
     *          { "full-white": [ "white" ] }
     *          { "full-white": [ "#FFFFFF" ] }
     *          { "half-white": [ "#FFFFFF7F" ] }
     *
     *   3. a JSON array like #2 but with an additional number within
     *      the range of 0 .. 255 OVERRIDING its alpha value,
     *
     *          { "full-white": [ "white", 255 ] }
     *          { "half-white": [ "white", 127 ] }
     *          { "full-white": [ "#FFFFFF", 255 ] }
     *          { "half-white": [ "#FFFFFF", 127 ] }
     *          { "full-white": [ "#FFFFFF7F", 255 ] }
     *          { "half-white": [ "#FFFFFF7F", 127 ] }
     *
     *   4. or a JSON array containing 3 (for RGB) OR 4 (for RGBA) numbers
     *      within the range of 0 .. 255.
     *
     *          { "full-white": [ 255, 255, 255 ] }
     *          { "half-white": [ 255, 255, 255, 127 ] }
     *
     */
    clan::Color getColor(std::string const &path) const;

    /** Loads a 2D vector object from the JSON file.
     *
     *  2D vectors are JSON arrays containing 2 numbers with the first
     *  element denoting position in the x-axis and the second element
     *  denoting position in the y-axis.
     *
     *  Valid JSON string:
     *      { "text-pos": [ 16, 32 ] }
     */
    clan::Vec2f getVec2f(std::string const &path) const;
    clan::Vec2i getVec2i(std::string const &path) const;

    /** Loads a 2D rectangle element from the JSON file.
     *
     *  2D rectangles can be represented in two different forms:
     *    - JSON array containing 4 numbers with the first two elements
     *      denoting the top-left 2D vector and the last two elements
     *      denoting the bottom-right 2D vector.
     *          { "box": [ 720, 540, 800, 600 ] }
     *    - JSON array containing 2 2D-vector arrays.
     *          { "box": [ [720, 540], [800, 600] ]}
     */
    clan::Rectf getRectf(std::string const &path) const;
    clan::Rect  getRecti(std::string const &path) const;

    /** Loads a font element from the JSON file.
     *
     *  A font object is simply a font description element (\see
     *  `getFontDesc`) with an additional property that is named either
     *  `name` or `path`. Having the `name` property on the element
     *  implies loading a font already installed inside the operating
     *  system while while having the `path` property implies loading
     *  a font from a `.ttf` file. The element must contain only one of
     *  either `name` and `path` properties or a `clan::JsonException`
     *  will be raised.
     *
     *  Valid JSON strings:
     *      { "fancy-header": { "name": "Tahoma Bold" , "height": 12 } }
     *      { "fancy-header": { "path": "tahomabd.ttf", "hegiht": 12 } }
     *
     */
    clan::Font getFont(std::string const &path) const;

    /** Loads a font description element from the JSON file.
     *
     *  Properties supported:
     *    - `height`    : Integer, typeface height value.
     *    - `weight`    : Integer, typeface weight value (thin, bold, etc.).
     *    - `italic`    : Boolean, determines if the typeface should be slanted.
     *    - `antialias` : Boolean, determines if anti-aliasing should be used
     *                             when rendering the font.
     *    - `subpixel`  : Boolean, determines if sub-pixel rendering should be
     *                             used to render the font.
     */
    clan::FontDescription getFontDesc(std::string const &path) const;

    /** Loads a font family element from the JSON file.
     *
     *  The final property name in the JSON path will be used as the
     *  font family name (i.e. if `path` supplied to this function is
     *  `skin.font.default.Tahoma`, then the font family name will be
     *  `Tahoma`).
     *
     *  A font family object must contain only objects within in. Each
     *  sub-object defines a font that would be added into the family
     *  and the name of the property will be used as the name of the
     *  font in the font family (**except** in case 3).
     *
     *  Fonts can be represented in three different forms:
     *
     *   1. an integer denoting the height of the font loaded,
     *
     *      { "DefaultFonts": { "Tahoma Bold": 12 } }
     *
     *   2. a FontDescription JSON object (see `getFontDesc`),
     *      { "Tahoma": {
     *          "Tahoma Regular": { "weight": 300 },
     *          "Tahoma Bold"   : { "weight": 700 }
     *      } }
     *
     *   3. or a FontDescription JSON object (see `getFontDesc`) with
     *      an additional string property `path` denoting the path to
     *      a `.ttf` file that will be loaded by ClanLib.
     *
     *      Note that the name of the font is ignored in this case.
     *
     *      { "Tahoma": {
     *          "Tahoma Regular": { "path": "tahoma.ttf"  , "height": 12, "weight": 300 },
     *          "Tahoma Bold"   : { "path": "tahomabd.ttf", "height": 12, "weight": 700 }
     *      } }
     *
     */
    clan::FontFamily getFontFamily(std::string const &path) const;


    /** Loads a number from the JSON file as a `double`. */
    double getDecimal(std::string const &path) const;

    /** Loads a number from the JSON file as an `int`. */
    int getInteger(std::string const &path) const;

    /** Loads a boolean value from the JSON file. */
    bool getBoolean(std::string const &path) const;

    /** Loads a string from the JSON file. */
    std::string getString(std::string const &path) const;

    template< class ValueType >
        ValueType get(
            const std::string &path,
            std::function< ValueType(const std::string &, const clan::JsonValue &) > function
        ) {
            return function(path, mRoot);
        }

    /** Retrieve if possible or overwrite with default value. */
    template< class ValueType, class ClassFunction >
        ValueType get_or_set(
            ClassFunction       func,
            std::string const & path,
            ValueType   const & default_value)
        {
            ValueType value;

            try {
                value = (this->*func)(path);
            } // TODO save value if fail
            catch ( std::out_of_range ) { value = default_value; }
            catch (clan::JsonException) { value = default_value; }

            return value;
        }

    /** Retrieve if valid or overwrite with default value. */
    template< class ValueType, class ClassFunction, class UnaryPredicate >
        ValueType get_if_else_set(
            ClassFunction       func,
            std::string const & path,
            ValueType   const & default_value,
            UnaryPredicate      p
            )
        {
            ValueType value;

            try {
                value = (this->*func)(path);
                value = p(value) ? value : default_value;
            } // TODO save value if fail
            catch ( std::out_of_range ) { value = default_value; }
            catch (clan::JsonException) { value = default_value; }

            return value;
        }
};

#endif
