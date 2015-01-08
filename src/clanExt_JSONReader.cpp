//  clanExt_JSONReader.cpp :: JSON reader
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

#include "clanExt_JSONReader.hpp"

#if CLANLIB_CURRENT_VERSION < CLANLIB_VERSION(4,0,0)
#error Requires ClanLib 4.0.0. Try an older version.
#endif

JSONReader::JSONReader() : mRoot() { }
JSONReader::JSONReader(clan::JsonValue const &root) : mRoot(root) { }

clan::JsonValue JSONReader::getJsonValue(std::string const &path, clan::JsonValue& root)
{
    if (path.empty()) return root;

    auto const end = path.find_first_of('.');
    auto const ind = path.find_first_of('[');

    bool has_dot   = end != std::string::npos;
    bool has_array = ind != std::string::npos;

    bool parse_array = has_array && (has_dot ? (ind < end) : true);

    // Get member with the `part' identifer
    std::string parsed_part = path.substr(0, end);
    std::string member_part = path.substr(0, parse_array ? ind : end);
    auto node = root;
    if (!member_part.empty()) {
        auto it = root.get_members().find(member_part);
        if (it == root.get_members().cend())
            throw std::out_of_range("Could not find member '" + member_part + "'");
        node = it->second;
    }

    if (parse_array)
    {
        if (node.is_array() == false)
            throw new clan::JsonException("Member '" + member_part + "' is not an array.");

        auto ind_end = parsed_part.find_first_of(']');
        if (ind_end == std::string::npos)
            throw new clan::JsonException("Expected closing bracket ']' in '" + parsed_part + "'.");

        if (ind_end <= ind)
            throw new clan::JsonException("Unexpected ']' in '" + parsed_part + "'.");

        std::string ind_part = parsed_part.substr(ind + 1, ind_end - 2);
        if (ind_part.empty())
            throw new clan::JsonException("Index enumerator in '" + parsed_part + "' is empty.");

        auto member = (node.get_items())[std::stoul(ind_part)];
        return has_dot ? getJsonValue(path.substr(end + 1), member) : member;
    }

    return has_dot ? getJsonValue(path.substr(end + 1), node) : node;
}

clan::JsonValue const JSONReader::cgetJsonValue(std::string const &path, clan::JsonValue const &root)
{
    if (path.empty()) return root;

    auto const end = path.find_first_of('.');
    auto const ind = path.find_first_of('[');

    bool has_dot   = end != std::string::npos;
    bool has_array = ind != std::string::npos;

    bool parse_array = has_array && (has_dot ? (ind < end) : true);

    // Get member with the `part' identifer
    std::string parsed_part = path.substr(0, end);
    std::string member_part = path.substr(0, parse_array ? ind : end);
    auto node = root;
    if (!member_part.empty()) {
        auto it = root.get_members().find(member_part);
        if (it == root.get_members().cend())
            throw std::out_of_range("Could not find member '" + member_part + "'");
        node = it->second;
    }

    if (parse_array)
    {
        if (node.is_array() == false)
            throw new clan::JsonException("Member '" + member_part + "' is not an array.");

        auto ind_end = parsed_part.find_first_of(']');
        if (ind_end == std::string::npos)
            throw new clan::JsonException("Expected closing bracket ']' in '" + parsed_part + "'.");

        if (ind_end <= ind)
            throw new clan::JsonException("Unexpected ']' in '" + parsed_part + "'.");

        std::string ind_part = parsed_part.substr(ind + 1, ind_end);
        if (ind_part.empty())
            throw new clan::JsonException("Index enumerator in '" + parsed_part + "' is empty.");

        auto const member = (node.get_items())[std::stoul(ind_part)];
        return has_dot ? cgetJsonValue(path.substr(end + 1), member) : member;
    }

    return has_dot ? cgetJsonValue(path.substr(end + 1), node) : node;
}

clan::Color JSONReader::getColor(std::string const &path) const
{
    clan::JsonValue const &json = cgetJsonValue(path, mRoot);

    if (json.is_string())
        // First form
        return clan::Color::find_color(json.to_string());

    if (json.is_array())
    {
        auto array = json.get_items();
        switch(array.size())
        {
            case 1:
                if (array[0].is_string())
                    // Second form
                    return clan::Color::find_color(json.to_string());
                break;

            case 2:
                if (array[0].is_string() && array[1].is_number())
                {
                    // Third form
                    clan::Color color = clan::Color::find_color(json.to_string());
                    color.set_alpha(array[1].to_int());
                    return color;
                }
                break;

            case 3:
                // Forth form
                return clan::Color(
                        array[0].to_int(),
                        array[1].to_int(),
                        array[2].to_int()
                        );

            case 4:
                // Forth form
                return clan::Color(
                        array[0].to_int(),
                        array[1].to_int(),
                        array[2].to_int(),
                        array[3].to_int()
                        );
        }
    }

    throw clan::JsonException(
            "Invalid JSON type while parsing color '" + path + "'."
            );

    return clan::Color();
}

clan::Vec2f JSONReader::getVec2f(std::string const &path) const
{
    clan::JsonValue const &json = cgetJsonValue(path, mRoot);

    if (json.is_array())
    {
        auto array = json.get_items();
        if (array.size() == 2)
            return clan::Vec2f(array[0].to_float(), array[1].to_float());
    }

    throw clan::JsonException(
            "Invalid JSON type while parsing 2D floating-point vector '" + path + "'."
            );

    return clan::Vec2f();
}

clan::Vec2i JSONReader::getVec2i(std::string const &path) const
{
    clan::JsonValue const &json = cgetJsonValue(path, mRoot);

    if (json.is_array())
    {
        auto array = json.get_items();
        if (array.size() == 2)
            return clan::Vec2i(array[0].to_int(), array[1].to_int());
    }

    throw clan::JsonException(
            "Invalid JSON type while parsing 2D integer vector '" + path + "'."
            );

    return clan::Vec2i();
}

clan::Rectf JSONReader::getRectf(std::string const &path) const
{
    clan::JsonValue const &json = cgetJsonValue(path, mRoot);

    if (json.is_array())
    {
        auto array = json.get_items();
        if (array.size() == 4)
            return clan::Rectf(
                    array[0].to_float(),
                    array[1].to_float(),
                    array[2].to_float(),
                    array[3].to_float()
                    );
        if (array.size() == 2)
            if (array[0].is_array() && array[1].is_array())
            {
                auto vecTL = array[0].get_items();
                auto vecBR = array[1].get_items();
                if (vecTL.size() && vecBR.size())
                    return clan::Rectf(
                            vecTL[0].to_float(),
                            vecTL[1].to_float(),
                            vecBR[0].to_float(),
                            vecBR[1].to_float()
                            );
            }
    }

    throw clan::JsonException(
            "Invalid JSON type while parsing 2D floating-point rectangle '" + path + "'."
            );

    return clan::Rectf();
}

clan::Rect JSONReader::getRecti(std::string const &path) const
{
    clan::JsonValue const &json = cgetJsonValue(path, mRoot);

    if (json.is_array())
    {
        auto array = json.get_items();
        if (array.size() == 4)
            return clan::Rect(
                    array[0].to_int(),
                    array[1].to_int(),
                    array[2].to_int(),
                    array[3].to_int()
                    );
        if (array.size() == 2)
            if (array[0].is_array() && array[1].is_array())
            {
                auto vecTL = array[0].get_items();
                auto vecBR = array[1].get_items();
                if (vecTL.size() && vecBR.size())
                    return clan::Rect(
                            vecTL[0].to_int(),
                            vecTL[1].to_int(),
                            vecBR[0].to_int(),
                            vecBR[1].to_int()
                            );
            }
    }

    throw clan::JsonException(
            "Invalid JSON type while parsing 2D integer rectangle '" + path + "'."
            );

    return clan::Rect();
}

clan::Font JSONReader::getFont(std::string const &path) const
{
    clan::JsonValue const &json = cgetJsonValue(path, mRoot);
    if (json.is_object())
    {
        // Grab font description.
        clan::FontDescription font_desc = this->getFontDesc(path);

        // Look for typeface name or path.
        auto node = json.get_members();
        auto const font_name = node.find("name");
        auto const font_path = node.find("path");

        /****/ if (font_name != node.cend() && font_path == node.cend()) {
            if (font_name->second.is_string()) // Font by name
                return clan::Font(font_name->second.to_string(), font_desc);
        } else if (font_name == node.cend() && font_path != node.cend()) {
            if (font_path->second.is_string()) // Font by path
                return clan::Font(font_desc, font_path->second.to_string());
        }
    }

    throw clan::JsonException(
            "Invalid JSON type while parsing font '" + path + "'."
            );

    return clan::Font();
}

clan::FontDescription JSONReader::getFontDesc(std::string const &path) const
{
    clan::FontDescription font_desc = clan::FontDescription();

    font_desc.set_subpixel(false);

    clan::JsonValue const &json = cgetJsonValue(path, mRoot);
    if (json.is_object())
    {
        for(auto node : json.get_members())
        {
            /****/ if (node.first.compare("height") == 0) {
                if (node.second.is_number())
                    font_desc.set_height(node.second.to_int());
                else
                    throw clan::JsonException(
                            "Invalid JSON type while parsing typeface height '" + path + "'."
                            );
            } else if (node.first.compare("weight") == 0) {
                if (node.second.is_number())
                    font_desc.set_weight(static_cast<clan::FontWeight>(node.second.to_int()));
                else
                    throw clan::JsonException(
                            "Invalid JSON type while parsing typeface weight '" + path + "'."
                            );
            } else if (node.first.compare("italic") == 0) {
                if (node.second.is_boolean())
                {
                    if (node.second.to_boolean())
                        font_desc.set_style(clan::FontStyle::italic);
                }
                else
                    throw clan::JsonException(
                            "Invalid JSON type while parsing typeface property '" + path + "'."
                            );
            } else if (node.first.compare("antialias") == 0) {
                if (node.second.is_boolean())
                    font_desc.set_anti_alias(node.second.to_boolean());
                else
                    throw clan::JsonException(
                            "Invalid JSON type while parsing typeface property '" + path + "'."
                            );
            } else if (node.first.compare("subpixel") == 0) {
                if (node.second.is_boolean())
                    font_desc.set_subpixel(node.second.to_boolean());
                else
                    throw clan::JsonException(
                            "Invalid JSON type while parsing typeface property '" + path + "'."
                            );
            } else { /* TODO Generate warning ? */ }
        }
    } else {
        throw clan::JsonException(
                "Invalid JSON type while parsing font descriptor '" + path + "'."
                );
    }

    return font_desc;
}

clan::FontFamily JSONReader::getFontFamily(std::string const &path) const
{
    clan::JsonValue const &json = cgetJsonValue(path, mRoot);
    if (json.is_object())
    {
        // Get family name from path.
        auto const dot = path.find_last_of('.');
        std::string family_name
            = (dot == std::string::npos)
                ? path
                : path.substr(dot + 1);

        // Load all fonts in font family.
        clan::FontFamily font_family(family_name);
        for(auto node : json.get_members())
        {
            /****/ if (node.second.is_object()) {
                auto font_desc = node.second.get_members();
                auto it = font_desc.find("path");
                if (it == font_desc.cend()) {
                    // clan::FontFamily(strTypefaceName, objFontDescription)
                    font_family.add(node.first, this->getFontDesc(path + "." + node.first));
                } else {
                    // clan::FontFamily(objFontDescription, strTTFPath)
                    font_family.add(this->getFontDesc(path + "." + node.first), it->second.to_string());
                }
            } else if (node.second.is_number()) {
                // clan::FontFamily(strTypefaceName, intHeight)
                font_family.add(node.first, node.second);
            } else {
                throw clan::JsonException(
                        "Invalid JSON type when parsing font face '" + path + "." + node.first + "'."
                        );
            }
        }

        return font_family;
    }

    throw clan::JsonException(
            "Invalid JSON type while parsing font family '" + path + "'."
            );

    return clan::FontFamily();
}


double JSONReader::getDecimal(std::string const &path) const
{
    return cgetJsonValue(path, mRoot).to_double();
}

int JSONReader::getInteger(std::string const &path) const
{
    return cgetJsonValue(path, mRoot).to_int();
}

bool JSONReader::getBoolean(std::string const &path) const
{
    return cgetJsonValue(path, mRoot).to_boolean();
}

std::string JSONReader::getString(std::string const &path) const
{
    return cgetJsonValue(path, mRoot).to_string();
}


