/*
Copyright (C) 2021  Kasyanov Nikolay Alexeevich (Unbewohnte (me@unbewohnte.xyz))

This file is part of broom.

broom is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

broom is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with broom.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>

// 2 pieces (middle and end of the file)
const uint8_t PIECE_SIZE = 16;

// A wrapper for every file with all necessary information
class Entry {
public:
    std::filesystem::path path;
    uintmax_t filesize;
    std::string pieces; // 2 hex-represented pieces of file

    Entry(const std::filesystem::path entry_path);
    ~Entry();

    // sets this entry`s filesize
    void get_size();

    // reads 2 pieces from the middle and the end of a file, converts them into
    // a convenient hex-encoded string
    void get_pieces();

    // REMOVE entry from the disk
    void remove();
};


#endif
