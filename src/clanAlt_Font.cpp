//  clanAlt_Font.cpp :: Alternate font module
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

#include "clanAlt_Font.hpp"
#include "clanExt_JSONFile.hpp"
#include <algorithm>

/*! EXPERIMENTAL Use algorithms that minimize font glyph map lookup.
 *  This might improve performance when dealing with very lengthy text.
 */
#define USE_MINIMAL_GLYPHMAP_LOOKUP


////    ATTRIBUTES    ////

//! \return The glyph metric mapped to the supplied character.
GlyphMetric SimpleFont::get_glyph_metric(const char& character) const
{
	auto it = mGlyphMap.find(character);
	if (it == mGlyphMap.end())
		return GlyphMetric{ .src_rect = clan::Rect{ 0, 0, 0, 0 }, .baseline = 0 };
	else
		return it->second;
}

//! Calculates the full Rect occupied for the supplied text.
clan::Rect SimpleFont::get_text_rect(const std::string& text) const
{
	clan::Rect rect = get_glyph_metric().get_rect();
	rect.left = rect.right = 0;
#ifdef USE_MINIMAL_GLYPHMAP_LOOKUP
	std::string str = text;
	while(!str.empty())
	{
		char ch = str[0];
		auto it = std::remove(str.begin(), str.end(), ch);
		auto nc = std::distance(it, str.end());
		rect.right += nc * get_glyph_metric(ch).src_rect.get_width();
		str.erase(it, str.end());
	}
#else
	for(const char &ch : text)
		rect.right += get_glyph_metric(ch).src_rect.get_width();
#endif
	return rect;
}


////    METHODS    ////

/*! Draws text onto a canvas.
 *  \param canvas Canvas to draw the text onto.
 *  \param point  Text baseline anchor position.
 *  \param text   Text to draw.
 *  \param color  Canvas blending color (\see clan::Canvas::fill_triangles).
 */
void SimpleFont::draw_text(clan::Canvas& canvas, const clan::Point& point, const std::string& text, const clan::Colorf& color) const
{
	clan::Point p = point;
	for(const char &ch : text)
	{
		GlyphMetric fg = get_glyph_metric(ch);
		clan::Quad  cq { fg.get_rect(p) };
		std::vector< clan::Vec2f > vec { cq.p, cq.s, cq.q, cq.q, cq.s, cq.r };
		canvas.fill_triangles( vec, mTexture, fg.src_rect, color );
		p.x += fg.src_rect.get_width();
	}
}


////    IMPLEMENTATION DETAIL    ////

/*! Loads a character-to-metric map from a JSON node.
 *
 *  \note Requires JSONReader module from clanExt.
 *
 *  A "glyph map" JSON node is simply a JSON array containing only "glyph" JSON
 *  nodes, which are also JSON arrays. Each "glyph" node has 3 parameters, all
 *  mandatory:
 *
 *  1. JSON string           -- The ASCII character (first char in the string)
 *                              to map.
 *  2. JSONReader Rect node  -- The location of the glyph bitmap in the texture.
 *  3. JSON integer          -- The baseline position of the glyph, relative to
 *                              the top of the Rect.
 *
 *  \param jPath    JSON path to the glyph map JSON object node, relative to the
 *                  input JsonValue object.
 *  \param jRoot    JSON tree to read from.
 *
 *  \return A GlyphMap object.
 */
SimpleFont::GlyphMap loadJSON_GlyphMap(const std::string &jPath, const clan::JsonValue &jRoot)
{
	clan::JsonValue const &root = JSONReader::cgetJsonValue(jPath, jRoot);
	if (!root.is_array()) {
		throw clan::JsonException(
				"Invalid JSON type while parsing font descriptor '" + jPath + "'."
				);
	}

	SimpleFont::GlyphMap map;
	for(auto node : root.get_items())
	{
		if (!node.is_array()) {
			throw clan::JsonException(
					"Invalid JSON type while parsing glyph metric '" + jPath + "'."
					);
		}

		char        gc;
		GlyphMetric gm;

		JSONReader json(node);

		gc          = json.getString ("[0]")[0];
		gm.src_rect = json.getRecti  ("[1]");
		gm.baseline = json.getInteger("[2]");

		map[gc] = gm;
	}

	return map;
}


////    CONSTRUCTORS    ////

//! Default empty SimpleFont object constructor
SimpleFont::SimpleFont() { }

/*! Loads a font from a JSON-formatted font description file.
 *  \param gc               Graphics context where the source texture will be stored in.
 *  \param font_desc_path   Path to JSON-formatted font description file.
 *  \return A SimpleFont object.
 */
SimpleFont SimpleFont::fromJSON(clan::GraphicContext &gc, const std::string &font_desc_path)
{
	JSONFile    json(font_desc_path);
	SimpleFont  font;
	font.mTexture  = clan::Texture2D(gc, json.getString("texture"));
	font.mGlyphMap = json.get<SimpleFont::GlyphMap>("glyphs", loadJSON_GlyphMap);

	return font;
}



