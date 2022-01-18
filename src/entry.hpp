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
#include <string>

#include "group.hpp"


namespace entry {
// 3 pieces (beginning, middle and end of the file)
const uint8_t PIECE_SIZE = 75;
const uint8_t PIECES_AMOUNT = 3;

// A wrapper for every file in filesystem with all necessary information
class Entry {
public:
    std::filesystem::path path; // set via constructor
    uintmax_t filesize; // set via constructor
    std::string pieces; // 3 hex-represented pieces of file; set only via a method call to not stress the disk
    group::Group group; // set externally

    Entry(const std::filesystem::path entry_path);
    ~Entry();

    // reads 3 pieces from the beginning, middle and the end of a file, converts them into
    // a convenient hex-encoded string. If a file has a size of less than PIECE_SIZE * PIECES_AMOUNT ->
    // constructs pieces from the whole file contents. If a file has no contents at all -> its pieces will be set to ""
    void get_pieces();

    // REMOVE entry from the disk
    void remove();
};

}



#endif
