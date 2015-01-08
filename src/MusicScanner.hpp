//  MusicScanner.hpp :: Music file scanner
//  Copyright 2013 Keigen Shu

#ifndef MUSIC_SCANNER_H
#define MUSIC_SCANNER_H

#include "Music.hpp"

MusicList scan_music_dir(std::string const &path);

bool msbyTitle (Music* const &lhs, Music* const &rhs);
bool msbyArtist(Music* const &lhs, Music* const &rhs);
bool msbyPath  (Music* const &lhs, Music* const &rhs);

#endif
