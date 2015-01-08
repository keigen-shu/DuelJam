//  clanAlt_Font.hpp :: Alternative font module
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

#ifndef H_CLAN_ALT_FONT
#define H_CLAN_ALT_FONT

#include <map>
#include <string>

#include <ClanLib/core.h>
#include <ClanLib/display.h>

//! Metrics structure for a single glyph of in a bitmap font.
struct GlyphMetric {
	/*! Glyph image rect on source texture.
	 *  src_rect.point  - Top-left position of source quad.
	 *  src_rect.width  - Glyph width.
	 *  src_rect.height - Glyph height. This should be the same in all glyphs.
	 *                    Otherwise, set this to the maximum glyph height for the
	 *                    NULL ('\0') character of the font.
	 */
	clan::Rect   src_rect;

	//! Glyph baseline / ascent; counted from the top of the glyph source rect.
	unsigned int baseline;

	inline unsigned int get_ascent() const { return baseline; };
	inline unsigned int get_descent() const { return src_rect.get_height() - baseline; }

	/*! Retrieves the glyph bounding box.
	 *
	 *  Point P = point;
	 *
	 *  p------q
	 *  | .##, |
	 *  | #  # |
	 *  P-###`-+ <- baseline
	 *  | #    |
	 *  s-'----r
	 */
	inline clan::Rect get_rect(const clan::Point &point = clan::Point { 0, 0 }) const
	{
		return clan::Rect {
				point.x                       , point.y - int(get_ascent ()),
				point.x + src_rect.get_width(), point.y + int(get_descent())
				};
	}
};

/*! Alternative bitmap font rendering class.
 *  Supports loading glyph data from a JSON file.
 *
 *  TODO Support multi-line rendering.
 *  TODO Support line truncating, like in original draw_text function.
 */
class SimpleFont {
public:
	typedef std::map< char, GlyphMetric > GlyphMap; //!< Character to Glyph Metric map type

private:
	clan::Texture2D mTexture;   //!< Font bitmap source texture
	GlyphMap        mGlyphMap;  //!< Character to Glyph Metric map

public: ////    CONSTRUCTORS    ////
	SimpleFont();
	static SimpleFont fromJSON(clan::GraphicContext &gc, const std::string &font_desc_path);

public: ////    ATTRIBUTES    ////
	GlyphMetric get_glyph_metric(const char &character = 0) const;
	clan::Rect get_text_rect(const std::string &text) const;

public: ////    METHODS    ////
	void draw_text(
			clan::Canvas &canvas,
			const clan::Point &point,
			const std::string &text,
			const clan::Colorf &color = clan::Colorf::white
			) const;

};
#endif
