//  MusicScanner.cpp :: Music file scanner
//  Copyright 2013 Keigen Shu

#include <ClanLib/core.h>
#include "MusicScanner.hpp"
#include "Chart_O2Jam.hpp"
#include "Chart_BMS.hpp"

MusicList scan_music_dir(std::string const &path)
{
    MusicList lwML;

    clan::DirectoryScanner clDS;
    if (clDS.scan(path))
    {
        while(clDS.next())
        {
            if (clDS.is_directory())
            {
                Music* music = scan_BMS_directory(
                    std::string(path) + "/" + clDS.get_name().c_str()
                );
                if (music != nullptr) lwML.push_back(music);
            }
        }
    } else {
        throw clan::Exception("Failed to open directory " + path);
    }

    if (clDS.scan(path, "*.ojn"))
    {
        while(clDS.next())
        {
            if (clDS.is_directory() == false)
            {
                Music* music = O2Jam::openOJN(
                    std::string(path) + "/" + clDS.get_name().c_str()
                );
                if (music != nullptr) lwML.push_back(music);
            }
        }
    } else {
        throw clan::Exception("Failed to open directory " + path);
    }

    lwML.sort(msbyPath);
    lwML.sort(msbyTitle);
    lwML.sort(msbyArtist);

    return lwML;
}

bool msbyTitle (Music* const &lhs, Music* const &rhs)
{
    return rhs->title.compare(lhs->title) > 0;
}

bool msbyArtist(Music* const &lhs, Music* const &rhs)
{
    return rhs->artist.compare(lhs->artist) > 0;
}

bool msbyPath  (Music* const &lhs, Music* const &rhs)
{
    return rhs->path.compare(lhs->path) > 0;
}

