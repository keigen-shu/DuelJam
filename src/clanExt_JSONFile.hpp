//  clanExt_JSONFile.hpp :: JSON file loader/saver
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

#ifndef H_CLAN_EXT_JSON_FILE
#define H_CLAN_EXT_JSON_FILE

#include "clanExt_JSONReader.hpp"

class JSONFile : public JSONReader
{
private:
    std::string const mFilePath;

public:
    JSONFile(std::string const &path);

    inline std::string const& getPath() { return mFilePath; }

    void load();
    void save();
};

#endif
