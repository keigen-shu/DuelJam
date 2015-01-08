//  clanExt_JSONFile.cpp :: JSON file loader/saver
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

#include "clanExt_JSONFile.hpp"

JSONFile::JSONFile(std::string const &path) :
    JSONReader(),
    mFilePath(path)
{
    load();
}

void JSONFile::load()
{
    mRoot = clan::JsonValue::from_json(clan::File::read_text(mFilePath));
}

void JSONFile::save()
{
    clan::File::write_text(mFilePath, mRoot.to_json());
}

