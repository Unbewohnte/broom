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

// 2 chunks (beginning and end of the file)
const uint8_t CHUNK_SIZE = 24;
const uint8_t CHECKSUM_SIZE = CHUNK_SIZE * 2;

// A wrapper for every file with all necessary information
class Entry {
public:
    std::filesystem::path path;
    uintmax_t filesize;
    char checksum[CHECKSUM_SIZE];


    Entry(const std::filesystem::path entry_path);
    ~Entry();

    // sets this entry`s filesize
    void get_size();

    // calculates and sets this entry`s checksum
    void get_checksum();

    // Compare this entry`s checksum with the other one.
    // If the checksums are the same -> returns true, else -> false
    bool compare_checksums(const char other_checksum[CHECKSUM_SIZE]);

    // REMOVE entry from the disk
    void remove();
};


#endif
