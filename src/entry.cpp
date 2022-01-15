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

#include <iostream>
#include <cerrno>
#include <cstring>


namespace entry {

// A wrapper for every file in filesystem with all necessary information
Entry::Entry(const std::filesystem::path entry_path) {
    // path
    path = entry_path;

    // filesize
    filesize = std::filesystem::file_size(path);
};

Entry::~Entry() {};

// reads 3 pieces from the beginning, middle and the end of a file, converts them into
// a convenient hex-encoded string
void Entry::get_pieces() {
    std::fstream entry_file;
    entry_file.open(path);

    if (!entry_file.is_open()) {
        throw std::ifstream::failure("Could not open \"" + path.string() + "\"; reason: " + std::string(std::strerror(errno)) + "\n");
    }

    char pieces_buffer[PIECE_SIZE * PIECES_AMOUNT];
    if (filesize <= PIECE_SIZE * PIECES_AMOUNT) {
        // can`t take whole 3 pieces !
        // read the whole file then
        entry_file.read(pieces_buffer, filesize);
    } else {
        // read chunk from the beginning
        char begin_buf[PIECE_SIZE];
        entry_file.read(begin_buf, PIECE_SIZE);
        for (uint8_t i = 0; i < PIECE_SIZE; i++) {
            pieces_buffer[i] = begin_buf[i];
        }

        uintmax_t middle_of_the_file = (double) filesize / 2.0 - PIECE_SIZE;

        entry_file.seekg(middle_of_the_file, std::ios::beg);
        // read CHUNK_SIZE bytes from the middle of the file
        char middle_buf[PIECE_SIZE];
        entry_file.read(middle_buf, PIECE_SIZE);
        for (uint8_t i = PIECE_SIZE; i < PIECE_SIZE * 2; i++) {
            pieces_buffer[i] = middle_buf[i - PIECE_SIZE];
        };

        // jump to the last CHUNK_SIZE bytes of the file and read the as well
        entry_file.seekg(PIECE_SIZE, std::ios::end);
        char end_buf[PIECE_SIZE];
        entry_file.read(end_buf, PIECE_SIZE);
        for (uint8_t i = PIECE_SIZE * 2; i < PIECE_SIZE * 3; i++) {
            pieces_buffer[i] = end_buf[i - PIECE_SIZE * 2];
        };
    };
    entry_file.close();

    // make a convenient hex string out of pure bytes
    std::stringstream pieces_hex;
    for (uint8_t i = 0; i < PIECE_SIZE * 2; i++) {
        pieces_hex << std::hex << static_cast<unsigned>(pieces_buffer[i]);
    };

    pieces = pieces_hex.str();
};

// Remove entry from the disk
void Entry::remove() {
    std::filesystem::remove(path);
};

}

