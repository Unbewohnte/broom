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

#include "entry.hpp"

// A wrapper for every file with all necessary information
Entry::Entry(const std::filesystem::path entry_path) {
    // path
    path = entry_path;
};

Entry::~Entry() {};

// sets this entry`s filesize
void Entry::get_size() {
    // filesize
    filesize = std::filesystem::file_size(path);
};

// calculates and sets this entry`s checksum
void Entry::get_checksum() {
    // checksum
    std::fstream entry_file;
    entry_file.open(path);

    if (!entry_file.is_open()) {
        throw std::ifstream::failure("Could not open \"" + path.filename().string() + "\"");
    }

    // TODO(Properly test it)
    if (filesize <= CHECKSUM_SIZE) {
        entry_file.read(checksum, CHECKSUM_SIZE);
    } else {
        char start_buf[CHUNK_SIZE];
        entry_file.read(start_buf, CHUNK_SIZE);

        entry_file.seekg(CHUNK_SIZE, std::ios::end);
        char end_buf[CHUNK_SIZE];
        entry_file.read(end_buf, CHUNK_SIZE);

        for (uint8_t i = 0; i < CHECKSUM_SIZE; i++) {
            if (i < CHUNK_SIZE) {
                checksum[i] = start_buf[i];
            }
            else if (i > CHUNK_SIZE) {
                checksum[i] = end_buf[i - CHUNK_SIZE];
            };
        };
    };

    entry_file.close();
};


// Compare this entry`s checksum with the other one.
// If the checksums are the same -> returns true, else -> false
bool Entry::compare_checksums(const char other_checksum[CHECKSUM_SIZE]) {
    for (uint8_t i = 0; i < CHECKSUM_SIZE; i++) {
        if (checksum[i] != other_checksum[i]) {
            return false;
        };
    };
    return true;
};

// Remove entry from the disk
void Entry::remove() {
    std::filesystem::remove(path);
};
