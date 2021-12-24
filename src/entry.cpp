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
class Entry {
public:
    Entry(std::filesystem::path path) {
        // check for existense and being a directory
        if (!std::filesystem::exists(path) || std::filesystem::is_directory(path)) {
            throw "Does not exist or a directory";
        };

        // filename
        filename = path.filename();

        // filesize
        filesize = std::filesystem::file_size(path);

        // checksum
        std::fstream entry_file;
        entry_file.open(path);

        if (!entry_file.is_open()) {
            throw "Could not open file";
        }

        char start_buf[CHUNK_SIZE];
        entry_file.read(start_buf, CHUNK_SIZE);

        char end_buf[CHUNK_SIZE];
        entry_file.read(end_buf, CHUNK_SIZE);

        char middle_buf[CHUNK_SIZE];
        entry_file.read(middle_buf, CHUNK_SIZE);

        for (uint8_t i = 0; i < CHECKSUM_SIZE; i++) {
            if (i < CHUNK_SIZE) {
                checksum[i] = start_buf[i];
            }
            else if (i > CHUNK_SIZE*2) {
                checksum[i] = middle_buf[i-(CHUNK_SIZE*2)];
            }
            else if (i > CHUNK_SIZE) {
                checksum[i] = end_buf[i - CHUNK_SIZE];
            }
        };
    };

    ~Entry() {};

    std::string filename;
    std::filesystem::path path;
    uintmax_t filesize;
    char checksum[CHECKSUM_SIZE];

    // Compare this entry`s checksum with the other one.
    // If the checksums are the same -> returns true, else -> false
    bool compare_checksums(char other_checksum[CHECKSUM_SIZE]) {
        for (uint8_t i = 0; i < CHECKSUM_SIZE; i++) {
            if (checksum[i] != other_checksum[i]) {
                return false;
            };
        };
        return true;
    };

    // Remove entity from the disk
    void remove() {
        std::filesystem::remove(path);
    };
};
